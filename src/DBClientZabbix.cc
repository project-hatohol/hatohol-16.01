/* Asura
   Copyright (C) 2013 MIRACLE LINUX CORPORATION
 
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "DBAgentSQLite3.h"
#include "DBClientZabbix.h"
#include "ItemEnum.h"
#include "ConfigManager.h"
#include "AsuraException.h"

const int DBClientZabbix::DB_VERSION = 1;

static const char *TABLE_NAME_SYSTEM = "system";
static const char *TABLE_NAME_REPLICA_GENERATION = "replica_generation";
static const char *TABLE_NAME_TRIGGERS_RAW_2_0 = "triggers_raw_2_0";

static const int REPLICA_GENERATION_NONE = -1;

static const ColumnDef COLUMN_DEF_SYSTEM[] = {
{
	ITEM_ID_NOT_SET,                   // itemId
	TABLE_NAME_SYSTEM,                 // tableName
	"version",                         // columnName
	SQL_COLUMN_TYPE_INT,               // type
	11,                                // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_NONE,                      // keyType
	0,                                 // flags
	NULL,                              // defaultValue
}, {
	ITEM_ID_NOT_SET,                   // itemId
	TABLE_NAME_SYSTEM,                 // tableName
	"latest_triggers_generation_id",   // columnName
	SQL_COLUMN_TYPE_INT,               // type
	11,                                // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_NONE,                      // keyType
	0,                                 // flags
	NULL,                              // defaultValue
}
};
static const size_t NUM_COLUMNS_SYSTEM =
  sizeof(COLUMN_DEF_SYSTEM) / sizeof(ColumnDef);

enum {
	IDX_SYSTEM_VERSION,
	IDX_SYSTEM_LATEST_TRIGGERS_GENERATION_ID,
	NUM_IDX_SYSTEM,
};

static const ColumnDef COLUMN_DEF_REPLICA_GENERATION[] = {
{
	ITEM_ID_NOT_SET,                   // itemId
	TABLE_NAME_REPLICA_GENERATION,     // tableName
	"replica_generation_id",           // columnName
	SQL_COLUMN_TYPE_INT,               // type
	11,                                // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_PRI,                       // keyType
	0,                                 // flags
	NULL,                              // defaultValue
}, {
	ITEM_ID_NOT_SET,                   // itemId
	TABLE_NAME_REPLICA_GENERATION,     // tableName
	"time",                            // columnName
	SQL_COLUMN_TYPE_BIGUINT,           // type
	20,                                // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_UNI,                       // keyType
	0,                                 // flags
	NULL,                              // defaultValue
}
};
static const size_t NUM_COLUMNS_REPLICA_GENERATION =
  sizeof(COLUMN_DEF_REPLICA_GENERATION) / sizeof(ColumnDef);


static const ColumnDef COLUMN_DEF_TRIGGERS_RAW_2_0[] = {
{
	ITEM_ID_NOT_SET,                   // itemId
	TABLE_NAME_TRIGGERS_RAW_2_0,       // tableName
	"replica_generation_id",           // columnName
	SQL_COLUMN_TYPE_INT,               // type
	11,                                // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_MUL,                       // keyType
	0,                                 // flags
	NULL,                              // defaultValue
}, {
	ITEM_ID_ZBX_TRIGGERS_TRIGGERID,    // itemId
	TABLE_NAME_TRIGGERS_RAW_2_0,       // tableName
	"triggerid",                       // columnName
	SQL_COLUMN_TYPE_BIGUINT,           // type
	20,                                // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_PRI,                       // keyType
	0,                                 // flags
	NULL,                              // defaultValue
}, {
	ITEM_ID_ZBX_TRIGGERS_EXPRESSION,   // itemId
	TABLE_NAME_TRIGGERS_RAW_2_0,       // tableName
	"expression",                      // columnName
	SQL_COLUMN_TYPE_VARCHAR,           // type
	255,                               // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_NONE,                      // keyType
	0,                                 // flags
	"",                                // defaultValue
}, {
	ITEM_ID_ZBX_TRIGGERS_DESCRIPTION,  // itemId
	TABLE_NAME_TRIGGERS_RAW_2_0,       // tableName
	"description",                     // columnName
	SQL_COLUMN_TYPE_VARCHAR,           // type
	255,                               // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_NONE,                      // keyType
	0,                                 // flags
	"",                                // defaultValue
}, {
	ITEM_ID_ZBX_TRIGGERS_URL,          // itemId
	TABLE_NAME_TRIGGERS_RAW_2_0,       // tableName
	"url",                             // columnName
	SQL_COLUMN_TYPE_VARCHAR,           // type
	255,                               // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_NONE,                      // keyType
	0,                                 // flags
	"",                                // defaultValue
}, {
	ITEM_ID_ZBX_TRIGGERS_STATUS,       // itemId
	TABLE_NAME_TRIGGERS_RAW_2_0,       // tableName
	"status",                          // columnName
	SQL_COLUMN_TYPE_INT,               // type
	11,                                // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_MUL,                       // keyType
	0,                                 // flags
	"0",                               // defaultValue
}, {
	ITEM_ID_ZBX_TRIGGERS_VALUE,        // itemId
	TABLE_NAME_TRIGGERS_RAW_2_0,       // tableName
	"value",                           // columnName
	SQL_COLUMN_TYPE_INT,               // type
	11,                                // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_MUL,                       // keyType
	0,                                 // flags
	"0",                               // defaultValue
}, {
	ITEM_ID_ZBX_TRIGGERS_PRIORITY,     // itemId
	TABLE_NAME_TRIGGERS_RAW_2_0,       // tableName
	"priority",                        // columnName
	SQL_COLUMN_TYPE_INT,               // type
	11,                                // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_NONE,                      // keyType
	0,                                 // flags
	"0",                               // defaultValue
}, {
	ITEM_ID_ZBX_TRIGGERS_LASTCHANGE,   // itemId
	TABLE_NAME_TRIGGERS_RAW_2_0,       // tableName
	"lastchange",                      // columnName
	SQL_COLUMN_TYPE_INT,               // type
	11,                                // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_NONE,                      // keyType
	0,                                 // flags
	"0",                               // defaultValue
}, {
	ITEM_ID_ZBX_TRIGGERS_COMMENTS,     // itemId
	TABLE_NAME_TRIGGERS_RAW_2_0,       // tableName
	"comments",                        // columnName
	SQL_COLUMN_TYPE_INT,               // type
	11,                                // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_NONE,                      // keyType
	0,                                 // flags
	NULL,                              // defaultValue
}, {
	ITEM_ID_ZBX_TRIGGERS_ERROR,        // itemId
	TABLE_NAME_TRIGGERS_RAW_2_0,       // tableName
	"error",                           // columnName
	SQL_COLUMN_TYPE_VARCHAR,           // type
	128,                               // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_NONE,                      // keyType
	0,                                 // flags
	"",                                // defaultValue
}, {
	ITEM_ID_ZBX_TRIGGERS_TEMPLATEID,   // itemId
	TABLE_NAME_TRIGGERS_RAW_2_0,       // tableName
	"templateid",                      // columnName
	SQL_COLUMN_TYPE_BIGUINT,           // type
	20,                                // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_MUL,                       // keyType
	0,                                 // flags
	NULL,                              // defaultValue
}, {
	ITEM_ID_ZBX_TRIGGERS_TYPE,         // itemId
	TABLE_NAME_TRIGGERS_RAW_2_0,       // tableName
	"type",                            // columnName
	SQL_COLUMN_TYPE_INT,               // type
	11,                                // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_NONE,                      // keyType
	0,                                 // flags
	"0",                               // defaultValue
}, {
	ITEM_ID_ZBX_TRIGGERS_VALUE_FLAGS,  // itemId
	TABLE_NAME_TRIGGERS_RAW_2_0,       // tableName
	"value_flags",                     // columnName
	SQL_COLUMN_TYPE_INT,               // type
	11,                                // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_NONE,                      // keyType
	0,                                 // flags
	"0",                               // defaultValue
}, {
	ITEM_ID_ZBX_TRIGGERS_FLAGS,        // itemId
	TABLE_NAME_TRIGGERS_RAW_2_0,       // tableName
	"flags",                           // columnName
	SQL_COLUMN_TYPE_INT,               // type
	11,                                // columnLength
	0,                                 // decFracLength
	false,                             // canBeNull
	SQL_KEY_NONE,                      // keyType
	0,                                 // flags
	"0",                               // defaultValue
}
};
static const size_t NUM_COLUMNS_TRIGGERS_RAW_2_0 =
   sizeof(COLUMN_DEF_TRIGGERS_RAW_2_0) / sizeof(ColumnDef);

struct DBClientZabbix::PrivateContext
{
	static GMutex mutex;
	static bool   dbInitializedFlags[NumMaxZabbixServers];
	DBAgent *dbAgent;

	// methods
	PrivateContext(void)
	: dbAgent(NULL)
	{
	}

	~PrivateContext()
	{
		if (dbAgent)
			delete dbAgent;
	}

	static void lock(void)
	{
		g_mutex_lock(&mutex);
	}

	static void unlock(void)
	{
		g_mutex_unlock(&mutex);
	}
};

GMutex DBClientZabbix::PrivateContext::mutex;
bool   DBClientZabbix::PrivateContext::dbInitializedFlags[NumMaxZabbixServers];

// ---------------------------------------------------------------------------
// Public methods
// ---------------------------------------------------------------------------
DBClientZabbix::DBClientZabbix(size_t zabbixServerId)
: m_ctx(NULL)
{
	ASURA_ASSERT(zabbixServerId < NumMaxZabbixServers,
	   "The specified zabbix server ID is larger than max: %d",
	   zabbixServerId); 
	m_ctx = new PrivateContext();

	m_ctx->lock();
	if (!m_ctx->dbInitializedFlags[zabbixServerId]) {
		// The setup function: dbSetupFunc() is called from
		// the constructor of DBAgentSQLite3() below.
		prepareSetupFuncCallback(zabbixServerId);
		m_ctx->dbInitializedFlags[zabbixServerId] = true;
	}
	m_ctx->unlock();

	DBDomainId domainId = DBDomainIDZabbixRawOffset + zabbixServerId;
	m_ctx->dbAgent = new DBAgentSQLite3(domainId);
}

DBClientZabbix::~DBClientZabbix()
{
	if (m_ctx)
		delete m_ctx;
}

void DBClientZabbix::addTriggersRaw2_0(ItemTablePtr tablePtr)
{
	m_ctx->dbAgent->begin();
	try {
		int newId = updateReplicaGeneration();
		addTriggersRaw2_0WithTryBlock(newId, tablePtr);
	} catch (...) {
		m_ctx->dbAgent->rollback();
		throw;
	}
	m_ctx->dbAgent->commit();
}

// ---------------------------------------------------------------------------
// Protected methods
// ---------------------------------------------------------------------------
void DBClientZabbix::dbSetupFunc(DBDomainId domainId)
{
	const string dbPath = DBAgentSQLite3::findDBPath(domainId);
	if (!DBAgentSQLite3::isTableExisting(dbPath, TABLE_NAME_SYSTEM))
		createTableSystem(dbPath);
	else
		updateDBIfNeeded(dbPath);

	if (!DBAgentSQLite3::isTableExisting(dbPath,
	                                     TABLE_NAME_REPLICA_GENERATION)) {
		createTableReplicaGeneration(dbPath);
	}

	if (!DBAgentSQLite3::isTableExisting(dbPath,
	                                     TABLE_NAME_TRIGGERS_RAW_2_0)) {
		createTableTriggersRaw2_0(dbPath);
	}
}

void DBClientZabbix::createTableSystem(const string &dbPath)
{
	DBAgentTableCreationArg arg;
	arg.tableName  = TABLE_NAME_SYSTEM;
	arg.numColumns = NUM_COLUMNS_SYSTEM;
	arg.columnDefs = COLUMN_DEF_SYSTEM;
	DBAgentSQLite3::createTable(dbPath, arg);

	// insert default value
	DBAgentInsertArg insArg;
	insArg.tableName = TABLE_NAME_SYSTEM;
	insArg.numColumns = NUM_COLUMNS_SYSTEM;
	insArg.columnDefs = COLUMN_DEF_SYSTEM;
	insArg.row->add(new ItemInt(DB_VERSION), false);
	insArg.row->add(new ItemInt(REPLICA_GENERATION_NONE), false);
	DBAgentSQLite3::insert(dbPath, insArg);
}

void DBClientZabbix::createTableReplicaGeneration(const string &dbPath)
{
	DBAgentTableCreationArg arg;
	arg.tableName  = TABLE_NAME_REPLICA_GENERATION;
	arg.numColumns = NUM_COLUMNS_REPLICA_GENERATION;
	arg.columnDefs = COLUMN_DEF_REPLICA_GENERATION;
	DBAgentSQLite3::createTable(dbPath, arg);
}

void DBClientZabbix::createTableTriggersRaw2_0(const string &dbPath)
{
	DBAgentTableCreationArg arg;
	arg.tableName  = TABLE_NAME_TRIGGERS_RAW_2_0;
	arg.numColumns = NUM_COLUMNS_TRIGGERS_RAW_2_0;
	arg.columnDefs = COLUMN_DEF_TRIGGERS_RAW_2_0;
	DBAgentSQLite3::createTable(dbPath, arg);
}

void DBClientZabbix::updateDBIfNeeded(const string &dbPath)
{
	if (getDBVersion(dbPath) == DB_VERSION)
		return;
	THROW_ASURA_EXCEPTION("Not implemented: %s", __PRETTY_FUNCTION__);
}

bool DBClientZabbix::getDBVersion(const string &dbPath)
{
	DBAgentSelectArg arg;
	arg.tableName = TABLE_NAME_SYSTEM;
	arg.columnDefs = COLUMN_DEF_SYSTEM;
	arg.columnIndexes.push_back(IDX_SYSTEM_VERSION);
	DBAgentSQLite3::select(dbPath, arg);

	const ItemGroupList &itemGroupList = arg.dataTable->getItemGroupList();
	ASURA_ASSERT(
	  itemGroupList.size() == 1,
	  "itemGroupList.size(): %zd", itemGroupList.size());
	ItemGroupListConstIterator it = itemGroupList.begin();
	const ItemGroup *itemGroup = *it;
	ASURA_ASSERT(
	  itemGroup->getNumberOfItems() == 1,
	  "itemGroup->getNumberOfItems: %zd", itemGroup->getNumberOfItems());
	ItemInt *itemVersion =
	  dynamic_cast<ItemInt *>(itemGroup->getItemAt(0));
	ASURA_ASSERT(itemVersion != NULL, "type: itemVersion: %s\n",
	             DEMANGLED_TYPE_NAME(*itemVersion));
	return itemVersion->get();
}

//
// Non-static methods
//
void DBClientZabbix::prepareSetupFuncCallback(size_t zabbixServerId)
{
	ConfigManager *configMgr = ConfigManager::getInstance();
	const string &dbDirectory = configMgr->getDatabaseDirectory();

	DBDomainId domainId = DBDomainIDZabbixRawOffset + zabbixServerId;
	DBAgent::addSetupFunction(DBDomainIDZabbixRawOffset + zabbixServerId,
	                          dbSetupFunc);
	string dbPath =
	  StringUtils::sprintf("%s/DBClientZabbix-%d.db",
	                       dbDirectory.c_str(), zabbixServerId);
	DBAgentSQLite3::defineDBPath(domainId, dbPath);
}

int DBClientZabbix::getLatestTriggersGenerationId(void)
{
	DBAgentSelectArg arg;
	arg.tableName = TABLE_NAME_SYSTEM;
	arg.columnDefs = COLUMN_DEF_SYSTEM;
	arg.columnIndexes.push_back(IDX_SYSTEM_LATEST_TRIGGERS_GENERATION_ID);
	m_ctx->dbAgent->select(arg);

	const ItemGroupList &itemGroupList = arg.dataTable->getItemGroupList();
	ASURA_ASSERT(
	  itemGroupList.size() == 1,
	  "itemGroupList.size(): %zd", itemGroupList.size());
	ItemGroupListConstIterator it = itemGroupList.begin();
	const ItemGroup *itemGroup = *it;
	ASURA_ASSERT(
	  itemGroup->getNumberOfItems() == 1,
	  "itemGroup->getNumberOfItems: %zd", itemGroup->getNumberOfItems());
	ItemInt *item =
	  dynamic_cast<ItemInt *>(itemGroup->getItemAt(0));
	ASURA_ASSERT(item != NULL, "type: itemVersion: %s\n",
	             DEMANGLED_TYPE_NAME(*item));
	return item->get();
}

int DBClientZabbix::updateReplicaGeneration(void)
{
	// We assumed that this function is called in the transcation.
	int id = getLatestTriggersGenerationId();
	int newId = id++;

	// insert the generation id
	uint64_t currTime = Utils::getCurrTimeAsMicroSecond();
	DBAgentInsertArg insertArg;
	insertArg.tableName = TABLE_NAME_REPLICA_GENERATION;
	insertArg.numColumns = NUM_COLUMNS_REPLICA_GENERATION;
	insertArg.columnDefs = COLUMN_DEF_REPLICA_GENERATION;
	insertArg.row->add(new ItemInt(newId), false);
	insertArg.row->add(new ItemUint64(currTime), false);

	// update the latest generation
	DBAgentUpdateArg updateArg;
	updateArg.tableName = TABLE_NAME_SYSTEM;
	updateArg.columnIndexes.push_back(
	  IDX_SYSTEM_LATEST_TRIGGERS_GENERATION_ID);
	updateArg.row->add(new ItemInt(newId), false);
	m_ctx->dbAgent->update(updateArg);

	return newId;
}

void DBClientZabbix::addTriggersRaw2_0WithTryBlock(int generationId,
                                                   ItemTablePtr tablePtr)
{
	// We assumed that this function is called in the transcation.

	const ItemGroupList &itemGroupList = tablePtr->getItemGroupList();
	ItemGroupListConstIterator it = itemGroupList.begin();
	for (; it != itemGroupList.end(); ++it) {
		const ItemGroup *itemGroup = *it;
		DBAgentInsertArg arg;
		arg.tableName = TABLE_NAME_TRIGGERS_RAW_2_0;
		arg.numColumns = NUM_COLUMNS_TRIGGERS_RAW_2_0;
		arg.columnDefs = COLUMN_DEF_TRIGGERS_RAW_2_0;

		arg.row->add(new ItemInt(generationId), false);
		for (size_t i = 0; i < itemGroup->getNumberOfItems(); i++)
			arg.row->add(itemGroup->getItemAt(i));
		m_ctx->dbAgent->insert(arg);
	}
}

