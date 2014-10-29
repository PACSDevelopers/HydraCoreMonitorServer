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
            'databaseStatus' => 'status',
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

    public static function alertDown($databaseTitle, $databaseID, $after, $ip, $dateCreated){
        $db = new \HC\DB();
        $users = $db->read('users', ['firstName', 'lastName', 'email'], ['notify' => 1]);
        if($users) {
            $email = new \HC\Email();
            $title = $databaseTitle . ' (' .  $databaseID . '): ' . 'Failed in ' . $after . 'ms on ' . $dateCreated;
            $message = '<br>' . $title . ' ' . $ip;
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
    
    public static function testMySQLPort($ip, $port = 3306) {
        $ip = ip2long($ip);
        if($ip) {
            if($fp = @fsockopen(long2ip($ip), $port)){
                fclose($fp);
                return true;
            }
        }
        
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
