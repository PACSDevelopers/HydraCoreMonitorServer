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
            'databaseIP' => 'ip',
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

        if(isset($data['ip'])) {
            $isIPValid = self::testMySQLPort($data['ip']);
            
            if(!$isIPValid) {
                $isValid = false;
            } else {
                $data['ip'] = ip2long($data['ip']);
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
            $data['Code Message'] = curl_strerror($data['Code']);
        }

        $db = new \HC\DB();
        $users = $db->read('users', ['firstName', 'lastName', 'email'], ['notify' => 1]);
        if($users) {
            $email = new \HC\Email();
            $title = $data['Database Title'] . ': ' . 'Failed (' . $data['Code']. ' - ' . $data['Code Message'] . ')';
            $message = new \HC\Table(['style' => 'width: 100%;']);
            $message->openHeader();
            $message->openRow();
            $message->column(['value' => 'Key']);
            $message->column(['value' => 'Value']);
            $message->closeRow();
            $message->closeHeader();
            $message->openBody();
            foreach($data as $key => $value) {
                $message->openRow();
                $message->column(['value' => $key]);
                $message->column(['value' => $value]);
                $message->closeRow();
            }
            $message->closeBody();

            foreach($users as $user) {
                $email->send($user['email'], $title, $message->render(), ['toName' => $user['firstName'] . ' ' . $user['lastName']]);
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
                    return false;
                }
            } else {
                $status = $db->update('database_backups', ['id' => $backupID], ['status' => 2, 'dateStarted' => $dateEdited, 'dateEdited' => $dateEdited]);
                if(!$status) {
                    return false;
                }
            }

            switch($type) {
                case 1:
                    $response =  self::runMySQLDumpDirectBackup($id, $ip, $path, $username, $password, $backupID);
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
            $db->update('database_backups', ['id' => $backupID], ['status' => 3, 'dateEnded' => $dateEdited, 'dateEdited' => $dateEdited]);
        } else {
            $db->update('database_backups', ['id' => $backupID], ['status' => 4, 'dateEnded' => $dateEdited, 'dateEdited' => $dateEdited]);
        }
        
        return $response;
    }
    
    public static function runMySQLDumpDirectBackup($id, $ip, $path, $username, $password, $backupID = 0) {
        if(is_file($path . '/' . $backupID . '.tar.xz')) {
            return true;
        }
        
        $backupDB = new \HC\DB(['databasename' => 'mysql', 'host' => $ip, 'username' => $username, 'password' => $password]);
        $schemas = $backupDB->query('SELECT 
                                        `table_schema`,
                                        SUM(`data_length` + `index_length`) as `size`
                                    FROM
                                        `information_schema`.`TABLES`
                                    GROUP BY table_schema;');
        $backupDB = null;
        if($schemas) {
            $schemaList = [];
            $dbSize = 0;
            
            foreach($schemas as $row) {
                if(!in_array($row['table_schema'], ['mysql', 'information_schema', 'performance_schema'])) {
                    $dbSize += $row['size'];
                    $schemaList[$row['table_schema']] = $row['size'];
                }
            }
                        
            if(!empty($schemaList)) {
                var_dump($path . '/' . $backupID);
                if (!is_dir($path . '/' . $backupID)) {
                    mkdir($path . '/' . $backupID);
                }

                $process = new \HC\Process();
                $processList = [];
                $schemaProcessMap = [];
                $startTime = time();
                
                foreach($schemaList as $schema => $schemaSize) {
                    $pid = $process->start('backup-' . $backupID . '-' . $schema . $startTime, 'mysqldump ' . $schema . ' --disable-keys --extended-insert --single-transaction --quick --max_allowed_packet=1G --compress --user=\'' . $username . '\' --password=\'' . $password . '\' -h ' . $ip . '  > ' . $path . '/' . $backupID . '/' . $schema . '.sql', $path, false);
                    if($pid) {
                        var_dump('backup-' . $backupID . '-' . $schema . $startTime);
                        $schemaProcessMap['backup-' . $backupID . '-' . $schema . $startTime] = $schema;
                        $processList['backup-' . $backupID . '-' . $schema . $startTime] = false;
                    } else {
                        var_dump('Shutdown', $pid);
                        
                        // Shutdown the backup
                        foreach($processList as $key => $value) {
                            $process->stop($key);
                        }
                        
                        // Clear up
                        $directory = new \HC\Directory();
                        $directory->delete($path . '/' . $backupID);
                        
                        return false;
                    }
                }

                var_dump($processList);
                
                $processCount = count($processList);
                $done = 0;
                $db = new \HC\DB();
                
                while($done != $dbSize) {
                    sleep(5);
                    var_dump('Checking List ' . $done . ' ' . $dbSize);
                    foreach($processList as $key => $value) {
                        if($processList[$key] === false) {
                            var_dump('Checking ' . $key);
                            if(!$process->isRunning($key)) {
                                var_dump('Done: ' . $key);
                                $process->stop($key);
                                $processList[$key] = true;
                                $curSize = $schemaList[$schemaProcessMap[$key]];
                                $done += $curSize;
                                $db->update('database_backups', ['id' => $backupID], ['progress' => (100 - ($dbSize - $done) / $dbSize * 100)]);
                            }                            
                        }
                    }
                }
                
                // Add an information file
                $info = ['id' => $backupID, 'ip' => $ip, 'dbSize' => $dbSize, 'schemas' => array_keys($schemaList), 'backupType' => 1];
                file_put_contents($path . '/' . $backupID . '/info.json', json_encode($info));
                
                var_dump('Compacting');
                
                // Compact final directory
                $command = 'cd ' . $path . '/' . $backupID . ' && tar cfk - * | pxz -1 -zfk - > ' . $path . '/' . $backupID . '.tar.xz';
                                
                $output = [];
                exec($command, $output, $returnCode);

                $directory = new \HC\Directory();
                $directory->delete($path . '/' . $backupID);
                
                if($returnCode === 0) {
                    return true;
                } else {
                    return false;
                }
            }
        }
        
        
        
        return true;
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
