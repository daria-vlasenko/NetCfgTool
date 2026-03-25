#include "profiles.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "core/paths.h"
#include "logger.h"

namespace {

constexpr const char *KEY_PROFILES = "profiles";

QString profilesFilePath()
{
    return NetCfgTool::Paths::profilesFile();
}

QJsonDocument loadJson(QString *error)
{
    const QString path = profilesFilePath();
    QFile f(path);
    if (!f.exists()) {
        return QJsonDocument(QJsonObject{
            { KEY_PROFILES, QJsonArray() }
        });
    }

    if (!f.open(QIODevice::ReadOnly)) {
        if (error) *error = QStringLiteral("Cannot open profiles file: ") + path;
        return QJsonDocument();
    }

    const QByteArray data = f.readAll();
    f.close();

    QJsonParseError pe;
    QJsonDocument doc = QJsonDocument::fromJson(data, &pe);
    if (pe.error != QJsonParseError::NoError) {
        if (error) *error = QStringLiteral("JSON parse error in profiles file: ") + pe.errorString();
        return QJsonDocument();
    }

    if (!doc.isObject()) {
        if (error) *error = QStringLiteral("Profiles file has invalid structure (expected JSON object).");
        return QJsonDocument();
    }

    return doc;
}

bool saveJson(const QJsonDocument &doc, QString *error)
{
    const QString path = profilesFilePath();
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error) *error = QStringLiteral("Cannot open profiles file for writing: ") + path;
        return false;
    }

    f.write(doc.toJson(QJsonDocument::Indented));
    f.close();
    return true;
}

QJsonObject profileToObject(const NetProfile &p)
{
    QJsonObject obj;
    obj["name"]    = p.name;
    obj["mode"]    = p.mode;
    obj["ipCidr"]  = p.ipCidr;
    obj["gateway"] = p.gateway;

    QJsonArray dnsArr;
    for (const QString &d : p.dns)
        dnsArr.append(d);
    obj["dns"] = dnsArr;

    return obj;
}

NetProfile objectToProfile(const QJsonObject &obj)
{
    NetProfile p;
    p.name    = obj.value("name").toString();
    p.mode    = obj.value("mode").toString();
    p.ipCidr  = obj.value("ipCidr").toString();
    p.gateway = obj.value("gateway").toString();

    const QJsonArray dnsArr = obj.value("dns").toArray();
    for (const QJsonValue &v : dnsArr)
        p.dns.append(v.toString());

    return p;
}

} // namespace

namespace Profiles {

bool save(const NetProfile &profile, QString *error)
{
    QString err;
    QJsonDocument doc = loadJson(&err);
    if (doc.isNull() && !err.isEmpty()) {
        if (error) *error = err;
        Logger::warn("Profiles::save loadJson failed: " + err);
        return false;
    }

    QJsonObject root = doc.object();
    QJsonArray arr   = root.value(KEY_PROFILES).toArray();

    bool updated = false;
    for (int i = 0; i < arr.size(); ++i) {
        QJsonObject obj = arr.at(i).toObject();
        if (obj.value("name").toString() == profile.name) {
            arr[i] = profileToObject(profile);
            updated = true;
            break;
        }
    }

    if (!updated)
        arr.append(profileToObject(profile));

    root[KEY_PROFILES] = arr;
    doc = QJsonDocument(root);

    if (!saveJson(doc, &err)) {
        if (error) *error = err;
        Logger::warn("Profiles::save saveJson failed: " + err);
        return false;
    }

    Logger::info("Profiles::save", QString("Profile %1 %2")
                 .arg(profile.name, updated ? "updated" : "created"));
    return true;
}

bool load(const QString &name, NetProfile &outProfile, QString *error)
{
    QString err;
    QJsonDocument doc = loadJson(&err);
    if (doc.isNull() && !err.isEmpty()) {
        if (error) *error = err;
        Logger::warn("Profiles::load loadJson failed: " + err);
        return false;
    }

    const QJsonObject root = doc.object();
    const QJsonArray  arr  = root.value(KEY_PROFILES).toArray();

    for (const QJsonValue &v : arr) {
        const QJsonObject obj = v.toObject();
        if (obj.value("name").toString() == name) {
            outProfile = objectToProfile(obj);
            Logger::info("Profiles::load", "Profile loaded: " + name);
            return true;
        }
    }

    if (error) *error = QStringLiteral("Profile not found: ") + name;
    Logger::warn("Profiles::load", "Profile not found: " + name);
    return false;
}

bool remove(const QString &name, QString *error)
{
    QString err;
    QJsonDocument doc = loadJson(&err);
    if (doc.isNull() && !err.isEmpty()) {
        if (error) *error = err;
        Logger::warn("Profiles::remove loadJson failed: " + err);
        return false;
    }

    QJsonObject root = doc.object();
    QJsonArray arr   = root.value(KEY_PROFILES).toArray();

bool found = false;
QJsonArray newArr;

    for (const QJsonValue &v : arr) {
        const QJsonObject obj  = v.toObject();
        const QString     pname = obj.value("name").toString();
        if (pname == name) {
            found = true;
            continue;
        }
        newArr.append(obj);
    }

    if (!found) {
        if (error) *error = QStringLiteral("Profile not found: ") + name;
        Logger::warn("Profiles::remove", "Profile not found: " + name);
        return false;
    }

    root[KEY_PROFILES] = newArr;
    doc = QJsonDocument(root);
    if (!saveJson(doc, &err)) {
        if (error) *error = err;
        Logger::warn("Profiles::remove saveJson failed: " + err);
        return false;
    }

    Logger::info("Profiles::remove", "Profile removed: " + name);
    return true;
}

QStringList list(QString *error)
{
    QString err;
    QJsonDocument doc = loadJson(&err);
    if (doc.isNull() && !err.isEmpty()) {
        if (error) *error = err;
        Logger::warn("Profiles::list loadJson failed: " + err);
        return {};
    }

    const QJsonObject root = doc.object();
    const QJsonArray  arr  = root.value(KEY_PROFILES).toArray();

    QStringList names;
    for (const QJsonValue &v : arr) {
        const QJsonObject obj = v.toObject();
        const QString name = obj.value("name").toString();
        if (!name.isEmpty())
            names.append(name);
    }

    names.sort();
    return names;
}

} // namespace Profiles
