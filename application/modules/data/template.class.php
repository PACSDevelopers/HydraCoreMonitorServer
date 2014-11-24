<?hh
namespace HCMS;

class Template extends \HC\Core
{    
    protected $db;
    protected $data = [];

    public function  __construct($template = [])
    {
        // Parse global / local options
        $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
        if(!isset($template['status'])) {
            $template['status'] = 1;
        }
        
        $this->db = new \HC\DB();
        $tempData = $this->db->read('data_templates', [], $template);
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
            'templateTitle' => 'title',
            'templateStatus' => 'status',
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

        if(!isset($data['editedBy'])) {
            $data['editedBy'] = $_SESSION['user']->getUserID();
        }

        if(!isset($data['dateEdited'])) {
            $data['dateEdited'] = time();
        }

        if($isValid) {
            $query = $this->db->update('data_templates', ['id' => $POST['data']['templateID']], $data);
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
        $query = $db->write('data_templates', $data);
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
