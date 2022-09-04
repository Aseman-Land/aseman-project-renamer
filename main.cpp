#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>

bool isBinary(const QByteArray &data)
{
    return memchr(data, '\0', data.length()) != NULL;
}

void rename(const QString &dir, const QString &from, const QString &to)
{
    auto dirs = QDir(dir).entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
    for (const auto &d: dirs)
    {

        const auto &path = dir + '/' + d;
        QFileInfo inf(path);
        if (inf.isFile())
        {
            QFile f(path);
            if (f.open(QFile::ReadOnly))
            {
                auto data = f.readAll();
                f.close();

                if (!isBinary(data.left(100)))
                {
                    data.replace(from.toUtf8(), to.toUtf8());
                    data.replace(from.toLower().toUtf8(), to.toLower().toUtf8());
                    data.replace(from.toUpper().toUtf8(), to.toUpper().toUtf8());

                    f.open(QFile::WriteOnly);
                    f.write(data);
                    f.close();
                }
            }
        }
        else
            rename(dir + '/' + d, from, to);

        auto new_d = d;
        new_d.replace(from, to);
        new_d.replace(from.toLower(), to.toLower());
        new_d.replace(from.toUpper(), to.toUpper());

        QDir(dir).rename(d, new_d);
    }

    auto files = QDir(dir).entryList(QDir::Files);
    for (const auto &d: dirs)
    {
        rename(dir + '/' + d, from, to);

        auto new_d = d;
        new_d.replace(from, to);
        new_d.replace(from.toLower(), to.toLower());
        new_d.replace(from.toUpper(), to.toUpper());

        QDir(dir).rename(d, new_d);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    const auto args = app.arguments();
    if (args.count() < 4)
    {
        qDebug() << "Usage:" << app.applicationName().toStdString().c_str() << "<folder>" << "<from_name>" << "<to_name>";
        return 0;
    }

    rename(args.at(1), args.at(2), args.at(3));

    return 0;
}
