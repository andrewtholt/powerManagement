
#include "plcBase.h"

class plcDatabase : public plcBase {
    private:
        sqlite3 *db; 
        string dbName=":memory:";
        bool sqlError(int rc, char *err_msg ) ;
        bool dbSetup() ;            // Use default database name.
        
        int con; // sqlite3 db connection

    public:
        plcDatabase();
        plcDatabase( string name);
        
        void setDBname( string );
        string getDBname( );
        
        virtual string getValue(string);
        virtual bool setValue(string, string );         // Short name, string value
    
        virtual string getOldValue(string );               // Short name
        virtual bool setOldValue(string, string );         // Short name, string value
};
