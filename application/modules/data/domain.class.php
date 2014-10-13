<?hh
namespace HCMS;

class Domain extends \HC\Core
{
    protected $db;
    protected $data = [];

    public function  __construct($domain = [])
    {
        // Parse global / local options
        $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
        if(!isset($domain['status'])) {
            $domain['status'] = 1;
        }
        
        $this->db = new \HC\DB();
        $tempData = $this->db->read('domains', [], $domain);
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
            'domainTitle' => 'title',
            'domainURL' => 'url',
            'domainStatus' => 'status',
        ];

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

        if(!isset($data['editedBy'])) {
            $data['editedBy'] = $_SESSION['user']->getUserID();
        }

        if(!isset($data['dateEdited'])) {
            $data['dateEdited'] = time();
        }

        $query = $this->db->update('domains', ['id' => $POST['data']['domainID']], $data);
        if($query){
            $response = ['status' => 1, 'dateEdited' => $data['dateEdited']];
        } else {
            $response['errors']['e4'] = true;
        }

        return $response;
    }

    public static function create($data){
        $db = new \HC\DB();
        $query = $db->write('domains', $data);
        if($query) {
            return new self(['id' => $db->getLastID()]);
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
