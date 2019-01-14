
#include "plcBase.h"

class plcDatabase : public plcBase {
    private:
        sqlite3 *db; 
        string dbName=":memory:";

    public:
        plcDatabase();
        
        void setDBname( string );
        string getDBname( );
        
        virtual string getValue(string);
        virtual bool setValue(string, string );         // Short name, string value
    
        virtual string getOldValue(string );               // Short name
        virtual bool setOldValue(string, string );         // Short name, string value
};
