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
    
    public static function checkHTTP($url, $returnCode = false, &$extraData = [], &$errorDetails = [], $key = false, $auth = false, $attempts = 1) {
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

            $headers = ['Host: ' . $url, 'X-Hc-Skip-App-Stats: 1', 'X-Requested-With: XMLHttpRequest'];
            if($key && $auth) {
                $headers[] = 'X-Hc-Auth-Code: ' . $auth->getCode($key);
            }

            curl_setopt($handle, CURLOPT_HTTPHEADER, $headers);
            curl_setopt($handle, CURLOPT_RETURNTRANSFER, true);
            curl_setopt($handle, CURLOPT_COOKIEJAR, $tempCookiesFile);
            curl_setopt($handle, CURLOPT_COOKIEFILE, $tempCookiesFile);
            curl_setopt($handle, CURLOPT_SSL_VERIFYPEER, true);
            curl_setopt($handle, CURLOPT_CONNECTTIMEOUT, 60);
            curl_setopt($handle, CURLOPT_TIMEOUT, 60);

            $curlResponse = curl_exec($handle);
            if($curlResponse) {
                $httpCode = curl_getinfo($handle, CURLINFO_HTTP_CODE);
                $extraData = curl_getinfo($handle);
            }
            
            $curlErrorCode = curl_errno($handle);
            if($curlErrorCode) {
                $httpCode = $curlErrorCode;
            }

            if($httpCode !== 200) {
                $json = json_decode($curlResponse, true);
                if($json) {
                    $errorDetails = $json;
                }
            }

            curl_close($handle);
            
            if($httpCode !== 200 && $attempts < 3) {
                $attempts++;
                return self::checkHTTP($url, $returnCode, $extraData, $errorDetails, $key, $auth, $attempts);
            } else if($returnCode) {
                return $httpCode;
            } else if($httpCode === 200) {
                return true;
            }
        }
        
        if($attempts < 3) {
            $attempts++;
            return self::checkHTTP($url, $returnCode, $extraData, $attempts);
        }
        
        return false;
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
            $title = $data['Domain Title'] . ': ' . 'Failed (' . $data['Code']. ' - ' . $data['Code Message'] . ')';
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
