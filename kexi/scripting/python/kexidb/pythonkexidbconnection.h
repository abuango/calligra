/***************************************************************************
 * pythonkexidbconnection.h
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#ifndef KROSS_PYTHONKEXIDBCONNECTION_H
#define KROSS_PYTHONKEXIDBCONNECTION_H

#include <Python.h>
#include "../CXX/Objects.hxx"
#include "../CXX/Extensions.hxx"

//#include <iostream>
#include <kexidb/drivermanager.h>
#include <kexidb/cursor.h>

namespace Kross
{

    class PythonKexiDBDriver;
    class PythonKexiDBConnectionData;
    class PythonKexiDBConnectionPrivate;

    /**
     * The PythonKexiDBConnection class is a from Py::Object inherited
     * object to represent the KexiDB::Connection class in python.
     */
    class PythonKexiDBConnection : public Py::PythonExtension<PythonKexiDBConnection>
    {
        public:
            PythonKexiDBConnection(PythonKexiDBDriver*, PythonKexiDBConnectionData*, KexiDB::Connection*);
            virtual ~PythonKexiDBConnection();

            virtual bool accepts(PyObject* pyobj) const;
            static void init_type(void);

        private:
            PythonKexiDBConnectionPrivate* d;

            Py::Object data(const Py::Tuple&);
            Py::Object driver(const Py::Tuple&);

            Py::Object connect(const Py::Tuple&);
            Py::Object isConnected(const Py::Tuple&);
            Py::Object disconnect(const Py::Tuple&);

            Py::Object isDatabase(const Py::Tuple&);
            Py::Object currentDatabase(const Py::Tuple&);
            Py::Object databaseNames(const Py::Tuple&);
            Py::Object isDatabaseUsed(const Py::Tuple&);
            Py::Object useDatabase(const Py::Tuple&);
            Py::Object closeDatabase(const Py::Tuple&);

            Py::Object tableNames(const Py::Tuple&);

            Py::Object executeQuery(const Py::Tuple&);
            Py::Object querySingleString(const Py::Tuple&);
            Py::Object queryStringList(const Py::Tuple&);

            Py::Object executeSQL(const Py::Tuple&);

            Py::Object insertRecord(const Py::Tuple&);
            Py::Object createDatabase(const Py::Tuple&);
            Py::Object dropDatabase(const Py::Tuple&);

            Py::Object createTable(const Py::Tuple&);
            Py::Object dropTable(const Py::Tuple&);

            //bool createTable( TableSchema* tableSchema, bool replaceExisting = false );
            //tristate dropTable( TableSchema* tableSchema );
            //tristate dropTable( const QString& table );
            //tristate alterTable( TableSchema& tableSchema, TableSchema& newTableSchema);
            //bool alterTableName(TableSchema& tableSchema, const QString& newName, bool replace = false);
            //QString selectStatement( QuerySchema& querySchema, int idEscaping = Driver::EscapeDriver|Driver::EscapeAsNecessary ) const;
    };

}

#endif

