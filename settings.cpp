#include "settings.h"

#include <QFile>
#include <qxmlstream.h>



void Settings::write(const QString &file, const QVariantMap &settings)
{
    QFile fp(file);
    fp.open(QFile::WriteOnly);
    QXmlStreamWriter writer(&fp);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    QVariantMap mymap=settings;
    QMap<QString,QStringList> pathMapc;
    for(auto key:mymap.keys())
    {
        pathMapc[key]=key.split('/');
    }
    std::function<void(QVariantMap&,int,QMap<QString,QStringList>&)> nest;
    nest=[&](QVariantMap&parent,int level,QMap<QString,QStringList>&pathMap){
        QMap<QString,QVariantMap> caches;
        for(auto key:pathMap.keys())
        {
            auto path=pathMap[key];
            if(path.size()-1<level)
                continue;
            if(path.size()-1==level)
            {
                parent[path[level]]=mymap[key];
                continue;
            }
            caches[path[level]][key]=mymap[key];
        }
        for(auto key:caches.keys())
        {
            QMap<QString,QStringList> subPath;
            for(auto sub:caches[key].keys())subPath[sub]=pathMap[sub];
            QVariantMap item;
            nest(item,level+1,subPath);
            parent[key]=item;
        }
    };
    QVariantMap result;
    nest(result,0,pathMapc);


    std::function<void(QVariantMap::iterator)> out;
    out=[&](QVariantMap::iterator item){

        writer.writeStartElement(item.key());
        auto node=item.value().toMap();
        for (auto key:node.keys()) {
            if(node[key].toMap().isEmpty())
                writer.writeAttribute(key,node[key].toString());
        }
        for (auto key:node.keys()) {
            if(!node[key].toMap().isEmpty())
                out(node.find(key));
        }
        writer.writeEndElement();
    };
    out(result.begin());
    writer.writeEndDocument();
    fp.close();
}

void Settings::read(const QString &file, QVariantMap &settings)
{
    QFile fp(file);
    fp.open(QFile::ReadOnly);
    QXmlStreamReader reader(&fp);
    std::function<void(QVariantMap&)> parse;
    parse=[&](QVariantMap&parent){

        if(reader.isStartElement())
        {
            parent["name"]=reader.name().toString();
            for(auto attr:reader.attributes())
            {
                parent[attr.name().toString()]=attr.value().trimmed().toString();
            }
        }
        QVariantMap list;
        while (reader.readNextStartElement()) {
            QVariantMap item;
            parse(item);
            if(item.size()>0)
            {
                auto section=item["name"].toString();
                for(auto key:item.keys())
                {
                    if(key=="name")continue;
                    auto rkey=section+"/"+key;
                    parent[rkey]=item[key];
                }
            }
        }
    };
    if(reader.readNextStartElement())
    {
        QVariantMap item;
        parse(item);
        if(item.size()>0)
        {
            auto section=item["name"].toString();
            for(auto key:item.keys())
            {
                if(key=="name")continue;
                auto rkey=section+"/"+key;
                settings[rkey]=item[key];
            }
        }
    }
    fp.close();
}

void Settings::sync(){
    QVariantMap map;
    auto it=cacheSettings.constBegin();
    while (it!=cacheSettings.constEnd()) {
        auto idx=it.key().lastIndexOf('/');
        auto section=it.key().left(idx);
        auto rkey=it.key().mid(idx+1);
        qDebug()<<section<<":"<<rkey;
        it++;
    }
}

void Settings::setValue(QString key, QVariant value){
    auto path=groupStack.join("/");
    cacheSettings[QString("%1/%2").arg(path).arg(key)]=value;
}

QVariant Settings::value(QString key){

    auto path=groupStack.join("/");
    return cacheSettings[QString("%1/%2").arg(path).arg(key)];
}

QStringList Settings::childGroups(){
    QMap<QString,QString> caches;
    if(groupStack.size()==0)
    {
        for(auto key:cacheSettings.keys())
        {
            if(key.contains("/"))
            {
                auto gkey=key.split("/")[0];
                if(!caches.contains(gkey))
                {
                    caches[gkey]=gkey;
                }
            }
        }
    }
    else {
        auto path=groupStack.join("/")+"/";
        for(auto key:cacheSettings.keys())
        {
            if(key.startsWith(path))
            {
                auto nkey=key.mid(path.size());
                if(nkey.contains("/"))
                {
                    auto gkey=nkey.split("/")[0];
                    if(!caches.contains(gkey))
                    {
                        caches[gkey]=gkey;
                    }
                }
            }
        }
    }
    return caches.keys();
}

QStringList Settings::childKeys(){
    QMap<QString,QString> caches;
    if(groupStack.size()==0)
    {
        return QStringList();
    }
    else {
        auto path=groupStack.join("/")+"/";
        for(auto key:cacheSettings.keys())
        {
            if(key.startsWith(path))
            {
                auto nkey=key.mid(path.size());
                if(!caches.contains(nkey))
                {
                    caches[nkey]=nkey;
                }
            }
        }
    }
    return caches.keys();
}



