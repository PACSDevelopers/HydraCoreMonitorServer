<?hh
namespace HCMS;

class Error extends \HC\Core
{    
    protected $db;
    protected $data = [];

    public function  __construct($error = [])
    {
        // Parse global / local options
        $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
        
        $this->db = new \HC\DB();
        $tempData = $this->db->read('errors', [], $error);
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

    public static function create($data){
        $db = new \HC\DB();
        $query = $db->write('errors', $data);
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
