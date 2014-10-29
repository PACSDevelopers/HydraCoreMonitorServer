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

        if(isset($data['url'])) {
            $httpCheck = self::checkHTTP($data['url']);
            if(!$httpCheck) {
                $isValid = false;
            }
        }

        if(!isset($data['editedBy'])) {
            $data['editedBy'] = $_SESSION['user']->getUserID();
        }

        if(!isset($data['dateEdited'])) {
            $data['dateEdited'] = time();
        }
        
        if($isValid) {
            $query = $this->db->update('domains', ['id' => $POST['data']['domainID']], $data);
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
        $query = $db->write('domains', $data);
        if($query) {
            return new self(['id' => $db->getLastID()]);
        }
        return false;
    }
    
    public static function checkHTTP($url, $returnCode = false, &$extraData = []) {
        $url = (string)$url;
        
        if(gethostbyname($url) !== $url) {
            $httpCode = false;
            $url = str_replace('http://', '', $url);
            $url = str_replace('https://', '', $url);
            $url = rtrim($url, '/');
            $handle = curl_init('http://' . $url);

            curl_setopt($handle, CURLOPT_FOLLOWLOCATION, true);
            $tempCookiesFile = sys_get_temp_dir() . '/' . md5((string)$url) . '.cookies';
            if(!is_file($tempCookiesFile)) {
                touch($tempCookiesFile);
            }

            curl_setopt($handle, CURLOPT_HTTPHEADER, ['X-Hc-Skip-App-Stats: 1']);
            curl_setopt($handle, CURLOPT_RETURNTRANSFER, true);
            curl_setopt($handle, CURLOPT_COOKIEJAR, $tempCookiesFile);
            curl_setopt($handle, CURLOPT_COOKIEFILE, $tempCookiesFile);
            curl_setopt($handle, CURLOPT_SSL_VERIFYPEER, true);

            $curlResponse = curl_exec($handle);
            if($curlResponse) {
                $httpCode = curl_getinfo($handle, CURLINFO_HTTP_CODE);
                $extraData = curl_getinfo($handle);
            }
            
            $curlErrorCode = curl_errno($handle);
            if($curlErrorCode) {
                $httpCode = $curlErrorCode;
            }

            curl_close($handle);
            
            if($returnCode) {
                return $httpCode;
            } else if($httpCode === 200) {
                return true;
            }
        }
        
        return false;
    }

    public static function alertDown($domainTitle, $domainID, $after, $url, $dateCreated){
        $db = new \HC\DB();
        $users = $db->read('users', ['email'], ['notify' => 1]);
        if($users) {
            $email = new \HC\Email();
            $title = $domainTitle . ' (' .  $domainID . '): ' . 'Failed in ' . $after . 'ms on ' . $dateCreated;
            $message = '<br>' . $title . ' ' . $url;
            foreach($users as $user) {
                $email->send($user['email'], $title, $message);
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
