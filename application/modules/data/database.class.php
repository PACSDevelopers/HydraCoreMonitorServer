<?hh
namespace HCMS;

class Database extends \HC\Core
{
    protected $db;
    protected $data = [];

    public function  __construct($database = [])
    {
        // Parse global / local options
        $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();

        $this->db = new \HC\DB();
        if(!isset($database['status'])) {
            $database['status'] = 1;
        }
        
        $tempData = $this->db->read('databases', [], $database);
        if($tempData) {
            $this->data = $tempData[0];
            $encryption = new \HC\Encryption();
            $this->data['username'] = $encryption->decrypt($this->data['username'], 'HC_DB_U' . $this->data['dateCreated']);
            $this->data['password'] = $encryption->decrypt($this->data['password'], 'HC_DB_P' . $this->data['dateCreated']);
        }
    }

    public function checkExists() {
        if(!empty($this->data)) {
            return true;
        }

        return false;
    }
    
    public function update($POST) {
        $response = ['errors' => []];

        $updateKeys = [
            'databaseTitle' => 'title',
            'databaseExtIP' => 'extIP',
            'databaseIntIP' => 'intIP',
            'databaseBackupType' => 'backupType',
            'databaseBackupInterval' => 'backupInterval',
            'databaseStatus' => 'status',
            'databaseUsername' => 'username',
            'databasePassword' => 'password'
        ];
        
        $isValid = true;
        $data = [];
        foreach($POST['data'] as $key => $value) {
            if(isset($updateKeys[$key])) {
                if(is_string($value)) {
                    $data[$updateKeys[$key]] = <x:frag>{$value}</x:frag>;
              } else {
                    $data[$updateKeys[$key]] = $value;
                }
            }
        }

        $isIPValid = true;
        if(isset($data['extIP'])) {
            $isIPValid = self::testMySQLPort($data['extIP']);

            if(!$isIPValid) {
                $isValid = false;
            } else {
                $data['extIP'] = ip2long($data['extIP']);
            }
        }
        
        if($isIPValid) {
            if(isset($data['intIP'])) {
                $isIPValid = self::testMySQLPort($data['intIP']);

                if(!$isIPValid) {
                    $isValid = false;
                } else {
                    $data['intIP'] = ip2long($data['intIP']);
                }
            }
        }

        if(isset($data['username']) ||  isset($data['password'])) {
            $encryption = new \HC\Encryption();
        }


        if(isset($data['username'])) {
            $data['username'] = $encryption->encrypt($data['username'], 'HC_DB_U' . $this->data['dateCreated']);
        }
        
        if(isset($data['password'])) {
            $data['password'] = $encryption->encrypt($data['password'], 'HC_DB_P' . $this->data['dateCreated']);
        }

        if(!isset($data['editedBy'])) {
            $data['editedBy'] = $_SESSION['user']->getUserID();
        }

        if(!isset($data['dateEdited'])) {
            $data['dateEdited'] = time();
        }

        if($isValid) {
            $query = $this->db->update('databases', ['id' => $POST['data']['databaseID']], $data);
            if($query){
                $response = ['status' => 1, 'dateEdited' => $data['dateEdited']];
            } else {
                $response['errors']['e4'] = true;
            }
        } else {
            $response['errors']['e5'] = true;
        }

        return $response;
    }

    public static function alertDown($data){
        $data = array_reverse($data, 1);

        if(isset(\HC\Error::$errorTitle[$data['Code']])) {
            $data['Code Message'] = \HC\Error::$errorTitle[$data['Code']];
        } else {
            $data['Code Message'] = \HC\Error::curl_strerror($data['Code']);
        }

        $db = new \HC\DB();
        $users = $db->read('users', ['firstName', 'lastName', 'email'], ['notify' => 1]);
        if($users) {
            $email = new \HC\Email();
            $title = $data['Database Title'] . ': ' . 'Failed (' . $data['Code']. ' - ' . $data['Code Message'] . ')';
            $tableBody = <tbody></tbody>;
            
            foreach($data as $key => $value) {
                $tableBody->appendChild(<tr>
                    <td>{$key}</td>
                    <td>{$value}</td>
                </tr>);
            }
            
            $message = <table style="width: 100%;">
                            <thead>
                                <tr>
                                    <th></th>
                                    <th></th>
                                </tr>
                            </thead>
                            {$tableBody}
                        </table>;
            
            $message = $message->__toString();
            
            foreach($users as $user) {
                $email->send($user['email'], $title, $message, ['toName' => $user['firstName'] . ' ' . $user['lastName']]);
            }
        }
        return false;
    }
    
    public static function create($data){
        $db = new \HC\DB();
        $query = $db->write('databases', $data);
        if($query) {
            return new self(['id' => $db->getLastID()]);
        }
        return false;
    }

    public static function testMySQLPort($ip, $port = 3306, $attempts = 1) {
        if($ip) {
            if($fp = @fsockopen($ip, $port)){
                fclose($fp);
                return true;
            }
        }

        if($attempts < 3) {
            $attempts++;
            return self::testMySQLPort($ip, $port, $attempts);
        }

        return false;
    }
    
    public static function runBackup($id, $ip, $path, $username, $password, $type, $backupID = 0) {
        $response = false;
        
        $df = disk_free_space($path);
        $dt = disk_total_space($path);
        $ds = 100 - ($df / $dt) * 100;
        
        if($ds < 90) {
            $db = new \HC\DB();

            $before = microtime(true);
            $dateTokens = explode('.', $before);
            if(!isset($dateTokens[1])) {
                $dateTokens[1] = 0;
            }

            $dateEdited = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

            if($backupID === 0) {
                $status = $db->insert('database_backups', ['databaseID' => $id, 'status' => 2, 'isLocal' => 1, 'dateStarted' => $dateEdited, 'dateCreated' => $dateEdited, 'dateEdited' => $dateEdited]);
                if($status) {
                    $backupID = $db->getLastID();
                } else {
                    throw new \Exception('Unable to write to database');
                }
            } else {
                $status = $db->update('database_backups', ['id' => $backupID], ['status' => 2, 'dateStarted' => $dateEdited, 'dateEdited' => $dateEdited]);
                if(!$status) {
                    throw new \Exception('Unable to write to database');
                }
            }

            switch($type) {
                case 1:
                    $response = self::runMySQLDumpDirectBackup($id, $ip, $path, $username, $password, $backupID);
                    break;
                case 2:
                    $response = self::runMySQLDumpClientBackup($id, $ip, $path, $backupID);
                    break;
                case 3:
                    $response = self::runInnoBackupExClientBackup($id, $ip, $path, $backupID);
                    break;
                default:
                    $response = false;
                    break;
            }
        }

        $before = microtime(true);
        $dateTokens = explode('.', $before);
        if(!isset($dateTokens[1])) {
            $dateTokens[1] = 0;
        }

        $dateEdited = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

        if($response) {
            $status = $db->update('database_backups', ['id' => $backupID], ['status' => 3, 'dateEnded' => $dateEdited, 'dateEdited' => $dateEdited]);
            if(!$status) {
                throw new \Exception('Unable to write to database');
            }
        } else {
            $status = $db->update('database_backups', ['id' => $backupID], ['status' => 4, 'dateEnded' => $dateEdited, 'dateEdited' => $dateEdited]);
            if(!$status) {
                throw new \Exception('Unable to write to database');
            }
        }
        
        return $response;
    }
    
    public function getDatabaseConnection($databasename = 'mysql') {
        $ip = long2ip($this->intIP);
        
        if(self::testMySQLPort($ip)) {
            try {
                return new \HC\DB(['databasename' => $databasename, 'host' => $ip, 'username' => $this->username, 'password' => $this->password]);
            } catch (\Exception $e) {}
        }

        $ip = long2ip($this->extIP);
        if(self::testMySQLPort($ip)) {
            try {
                return new \HC\DB(['databasename' => $databasename, 'host' => $ip, 'username' => $this->username, 'password' => $this->password]);
            } catch (\Exception $e) {}
        }
        
        return false;
    }
    
    public function getSchema($name) {
        $tempDB = $this->getDatabaseConnection('INFORMATION_SCHEMA');
        if($tempDB) {
            $tables = $tempDB->read('COLUMNS', ['TABLE_NAME', 'COLUMN_NAME'], ['TABLE_SCHEMA' => $name]);
            $schema = [];
            if($tables) {
                foreach($tables as $row) {
                    if(!isset($schema[$row['TABLE_NAME']])) {
                        $schema[$row['TABLE_NAME']] = [$row['COLUMN_NAME']];
                    } else {
                        $schema[$row['TABLE_NAME']][] = $row['COLUMN_NAME'];
                    }
                    
                }
            }
            
            return $schema;
        }
        
        return false;
    }
    
    public function getSchemas() {
        $connection = $this->getDatabaseConnection();
        if($connection) {
            $result = $connection->query('SELECT `SCHEMA_NAME` FROM `INFORMATION_SCHEMA`.`SCHEMATA` WHERE `SCHEMA_NAME` NOT IN(?,?,?);', ['information_schema', 'performance_schema', 'mysql']);
            if($result) {
                $niceResult = [];

                foreach($result as $row) {
                    $niceResult[] = $row['SCHEMA_NAME'];
                }
                
                return $niceResult;
            }
        }
        
        return false;
    }
    
    public static function runMySQLDumpDirectBackup($id, $ip, $path, $username, $password, $backupID = 0) {
        if(is_file($path . '/' . $backupID . '.tar.xz')) {
            return true;
        }

        $directory = new \HC\Directory();
        
        $backupDB = new \HC\DB(['databasename' => 'mysql', 'host' => $ip, 'username' => $username, 'password' => $password]);
        $schemas = $backupDB->query('SELECT 
                                        `table_schema`,
                                        SUM(`data_length` + `index_length`) as `size`
                                    FROM
                                        `information_schema`.`TABLES`
                                    GROUP BY table_schema;');
        
        if($schemas) {
            $schemaList = [];
            $schemaCreateSyntaxs = [];
            $dbSize = 0;
            
            foreach($schemas as $row) {
                if(!in_array($row['table_schema'], ['mysql', 'information_schema', 'performance_schema'])) {
                    $create = $backupDB->query('SHOW CREATE DATABASE `' . $row['table_schema'] . '`');
                    if($create) {
                        $dbSize += $row['size'];
                        $schemaList[$row['table_schema']] = $row['size'];
                        $schemaCreateSyntaxs[$row['table_schema']] = $create[0]['Create Database'];
                    }
                }
            }

            $backupDB = null;
            
            if(!empty($schemaList)) {
                if (!is_dir($path . '/' . $backupID)) {
                    mkdir($path . '/' . $backupID);
                }

                $process = new \HC\Process();
                $processList = [];
                $schemaProcessMap = [];
                $startTime = time();
                
                foreach($schemaList as $schema => $schemaSize) {
                    $pid = $process->start('backup-' . $backupID . '-' . $schema . $startTime, 'mysqldump ' . $schema . ' --disable-keys --extended-insert --single-transaction --quick --max_allowed_packet=1G --compress --user=\'' . $username . '\' --password=\'' . $password . '\' -h ' . $ip . '  > ' . $path . '/' . $backupID . '/' . $schema . '.sql', $path, false, false);
                    if($pid) {
                        $schemaProcessMap['backup-' . $backupID . '-' . $schema . $startTime] = $schema;
                        $processList['backup-' . $backupID . '-' . $schema . $startTime] = false;
                    } else {
                        
                        // Shutdown the backup
                        foreach($processList as $key => $value) {
                            $process->stop($key);
                        }
                        
                        // Clear up
                        $directory->delete($path . '/' . $backupID);

                        throw new \Exception('Unable to start process');
                        return false;
                    }
                }

                
                $processCount = count($processList);
                $done = 0;
                $db = new \HC\DB();
                
                while($done != $dbSize) {
                    sleep(5);
                    $tempRow = $db->read('database_backups', ['status'], ['id' => $backupID]);
                    if($tempRow) {
                        if($tempRow[0]['status'] != 2) {
                            // Shutdown the transfer
                            foreach($processList as $key => $value) {
                                $process->stop($key);
                            }
                            
                            $directory->delete($path . '/' . $backupID);
                            return false;
                        } else {
                            foreach($processList as $key => $value) {
                                if($processList[$key] === false) {
                                    if(!$process->isRunning($key)) {
                                        $process->stop($key);
                                        $processList[$key] = true;
                                        $done += $schemaList[$schemaProcessMap[$key]];

                                        $before = microtime(true);
                                        $dateTokens = explode('.', $before);
                                        if(!isset($dateTokens[1])) {
                                            $dateTokens[1] = 0;
                                        }

                                        $dateEdited = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

                                        $status = $db->update('database_backups', ['id' => $backupID], ['dateEdited' => $dateEdited, 'progress' => floor(100 - ($dbSize - $done) / $dbSize * 100)]);
                                        if(!$status) {
                                            $directory->delete($path . '/' . $backupID);
                                            throw new \Exception('Unable to write to database');
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                }
                
                // Add an information file
                $info = ['id' => $backupID, 'extIP' => $ip, 'dbSize' => $dbSize, 'schemas' => array_keys($schemaList), 'schemaCreateSyntaxs' => $schemaCreateSyntaxs, 'backupType' => 1];
                file_put_contents($path . '/' . $backupID . '/info.json', json_encode($info));
                
                // Compact final directory
                $command = 'cd ' . $path . '/' . $backupID . ' && tar cfk - * | pxz -1 -zfk - > ' . $path . '/' . $backupID . '.tar.xz';
                                
                $output = [];
                exec($command, $output, $returnCode);

                $directory->delete($path . '/' . $backupID);
                
                if($returnCode === 0) {
                    return true;
                } else {
                    throw new \Exception('Unable to compress file');
                    return false;
                }
            } else {
                return true;
            }
        } else {
            return true;
        }


        throw new \Exception('Unknown Error');
        return false;
    }

    public static function transferBackup($transferID, $path, $backupID, $ip, $username, $password) {
        $db = new \HC\DB();
        $transferDB = new \HC\DB(['databasename' => 'mysql', 'host' => $ip, 'username' => $username, 'password' => $password]);
        $directory = new \HC\Directory();
        
        if(!is_dir($path . '/' . $backupID)) {
            mkdir($path . '/' . $backupID);
        } else {
            $directory->delete($path . '/' . $backupID);
            mkdir($path . '/' . $backupID);
        }
        
        $command = 'cd ' . $path . '/' . $backupID . ' && tar -kxJf ' . $path . '/' . $backupID . '.tar.xz';

        $output = [];
        exec($command, $output, $returnCode);

        if($returnCode === 0) {
            $info = json_decode(file_get_contents($path . '/' . $backupID . '/info.json'), true);
            
            if($info) {
                $schemaList = [];
                $schemaCount = count($info['schemas']);
                $schemasLeft = $schemaCount;
                $dbSize = 0;
                
                $process = new \HC\Process();
                $processList = [];
                $schemaProcessMap = [];
                
                foreach($info['schemas'] as $schema) {
                    $schemaList[$schema] = filesize($path . '/' . $backupID . '/' . $schema . '.sql');
                    $dbSize += $schemaList[$schema];
                    
                    $transferDB->query('DROP DATABASE IF EXISTS `' . $schema . '`;', [], -1, true);
                    $transferDB->query($info['schemaCreateSyntaxs'][$schema], [], -1, true);

                    $pid = $process->start('transfer-' . $transferID . '-' . $schema, 'mysql --user=\'' . $username . '\' --password=\'' . $password . '\' -h ' . $ip . ' ' . $schema . ' < ' . $path . '/' . $backupID . '/' . $schema . '.sql', $path, false, false);
                    if($pid) {
                        $schemaProcessMap['transfer-' . $transferID . '-' . $schema] = $schema;
                        $processList['transfer-' . $transferID . '-' . $schema] = false;
                    } else {

                        // Shutdown the backup
                        foreach($processList as $key => $value) {
                            $process->stop($key);
                        }

                        // Clear up
                        $directory->delete($path . '/' . $backupID);

                        throw new \Exception('Unable to start process');
                        return false;
                    }
                }
                
                $processCount = count($processList);
                $done = 0;

                while($done != $dbSize) {
                    sleep(5);
                    $tempRow = $db->read('database_transfers', ['status'], ['id' => $transferID]);
                    if($tempRow) {
                        if($tempRow[0]['status'] != 2) {
                            // Shutdown the backup
                            foreach($processList as $key => $value) {
                                $process->stop($key);
                            }

                            // Clear up
                            $directory->delete($path . '/' . $backupID);
                            
                            return false;
                        } else {
                            foreach($processList as $key => $value) {
                                if($processList[$key] === false) {
                                    if(!$process->isRunning($key)) {
                                        $process->stop($key);
                                        $processList[$key] = true;
                                        $done += $schemaList[$schemaProcessMap[$key]];
                                        $status = $db->update('database_transfers', ['id' => $transferID], ['progress' => floor(100 - ($dbSize - $done) / $dbSize * 100)]);
                                        if(!$status) {
                                            // Shutdown the backup
                                            foreach($processList as $key => $value) {
                                                $process->stop($key);
                                            }

                                            // Clear up
                                            $directory->delete($path . '/' . $backupID);

                                            throw new \Exception('Unable to write to database');

                                            return false;
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        // Shutdown the backup
                        foreach($processList as $key => $value) {
                            $process->stop($key);
                        }

                        // Clear up
                        $directory->delete($path . '/' . $backupID);

                        return false;
                    }                    
                }

                $directory->delete($path . '/' . $backupID);
                return true;
            } else {
                throw new \Exception('Unable to find info');
            }
        } else {
            throw new \Exception('Unable to start process');
        }
        
        $directory->delete($path . '/' . $backupID);
        throw new \Exception('Unknown error');
        return false;
    }
    
    public static function runMySQLDumpClientBackup($id, $ip, $path, $backupID = 0) {
        return false;
    }

    public static function runInnoBackupExClientBackup($id, $ip, $path, $backupID = 0) {
        return false;
    }
        
    public function __set($key, $value)
    {
        $this->data[$key] = $value;
        return true;
    }

    function __get($key) {
        if (array_key_exists($key, $this->data)) {
            return $this->data[$key];
        }

        return false;
    }

    public function __isset($key)
    {
        return isset($this->data[$key]);
    }

    public function __unset($key)
    {
        unset($this->data[$key]);
    }
}
