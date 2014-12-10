<?hh
namespace HCMS;

class Issue extends \HC\Core
{    
    protected $db;
    protected $data = [];

    public function  __construct($issue = [])
    {
        // Parse global / local options
        $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
        if(!isset($issue['status'])) {
            $issue['status'] = 1;
        }
        
        $this->db = new \HC\DB();
        $tempData = $this->db->read('issues', [], $issue);
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
            'issueCode' => 'code',
            'issueStatus' => 'status',
            'issueDateClosed' => 'dateClosed',
            'issueDateLastConfirmed' => 'dateLastConfirmed',
            'issueServerID' => 'serverID',
            'issueDomainID' => 'domainID',
            'issueDatabaseID' => 'databaseID',
            'issueErrorID' => 'errorID',
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

        if($isValid) {
            $query = $this->db->update('issues', ['id' => $POST['data']['issueID']], $data);
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

    public static function create($data){
        $db = new \HC\DB();
        $query = $db->write('issues', $data);
        if($query) {
            return new self(['id' => $db->getLastID()]);
        }
        return false;
    }

    public function acknowledge() {
        return $this->db->update('issues', ['id' => $this->id], ['status' => 2]);
    }

    public function resolve() {
        return $this->db->update('issues', ['id' => $this->id], ['status' => 3, 'dateClosed' => time()]);
    }

    public function confirm() {
        $time = time();
        if($this->db->update('issues', ['id' => $this->id], ['dateLastConfirmed' => $time])) {
            $this->dateLastConfirmed = $time;
            return true;
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
