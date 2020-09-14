#include "csettings.h"

#include <qxmlstream.h>
#include <QIODevice>


bool xmlWrite(QIODevice &fp, const QVariantMap &settings)
{
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
    return true;
}

bool xmlRead(QIODevice&fp, QVariantMap &settings)
{
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
    return true;
}

QSettings::Format xmlFormat=QSettings::registerFormat("xml",xmlRead,xmlWrite);



CSettings::CSettings(const QString&fileName,QObject *parent) :
    QSettings(fileName,xmlFormat,parent)
{

}
