<?hh
namespace HCMS;

class Server extends \HC\Core
{    
    protected $db;
    protected $data = [];

    public function  __construct($server = [])
    {
        // Parse global / local options
        $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
        if(!isset($server['status'])) {
            $server['status'] = 1;
        }
        
        $this->db = new \HC\DB();
        $tempData = $this->db->read('servers', [], $server);
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
            'serverTitle' => 'title',
            'serverIP' => 'ip',
            'serverStatus' => 'status',
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
            $data['ip'] = ip2long($data['ip']);
            if($data['ip']) {
                $check = self::checkHTTP($data['ip'], $data['ip']);
                if(!$check) {
                    $isValid = false;
                }
            } else {
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
            $query = $this->db->update('servers', ['id' => $POST['data']['serverID']], $data);
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

    public static function checkHTTP($ip, $url, $returnCode = false, &$extraData = [], &$errorDetails = [], $key = false, $auth = false, $prefix = 'http', $suffix = '', $cookies = [], $trips = 0, $attempts = 1) {
        $httpCode = false;
        $port = 80;
        if($prefix === 'https') {
            $port = 443;
        }

        if($trips >= 20) {
            return false;
        }

        $handle = curl_init();

        $tempCookiesFile = sys_get_temp_dir() . '/' . md5($url . $ip) . '.cookies';
        if(!is_file($tempCookiesFile)) {
            file_put_contents($tempCookiesFile, json_encode($cookies));
        } else if(empty($cookies)) {
            $cookies = (array)json_decode(file_get_contents($tempCookiesFile));
        }

        curl_setopt($handle, CURLOPT_URL, $prefix . '://' . $ip . $suffix);
        curl_setopt($handle, CURLOPT_FOLLOWLOCATION, false);
        curl_setopt($handle, CURLOPT_HEADER, true);
        curl_setopt($handle, CURLOPT_RETURNTRANSFER, true);
        $headers = ['Host: ' . $url, 'X-Hc-Skip-App-Stats: 1', 'X-Requested-With: XMLHttpRequest'];
        if($key && $auth) {
            $headers[] = 'X-Hc-Auth-Code: ' . $auth->getCode($key);
        }
        
        curl_setopt($handle, CURLOPT_HTTPHEADER, $headers);
        curl_setopt($handle, CURLOPT_SSL_VERIFYPEER, false);
        curl_setopt($handle, CURLOPT_SSL_VERIFYHOST, false);
        curl_setopt($handle, CURLOPT_COOKIE, http_build_query($cookies));
        curl_setopt($handle, CURLOPT_CONNECTTIMEOUT, 60);
        curl_setopt($handle, CURLOPT_TIMEOUT, 60);

        $curlResponse = curl_exec($handle);
        
        if($curlResponse) {
            $extraData = curl_getinfo($handle);
            $httpCode = $extraData['http_code'];
            if($httpCode === 301 || $httpCode === 302) {
                $oldCookies = $cookies;
                $matches = [];
                preg_match_all('/^Set-Cookie: (.*?)=(.*?);/m', $curlResponse, $matches);
                if(isset($matches[1])) {
                    foreach($matches[1] as $index => $cookie) {
                        $cookies[$cookie] = $matches[2][$index];
                    }
                }
                if($cookies !== $oldCookies) {
                    file_put_contents($tempCookiesFile, json_encode($cookies));
                }

                $matches = [];
                $result = preg_match('/(Location:|URI:)(.*?)\n/', $curlResponse, $matches);
                if(isset($matches[2])) {
                    $location = trim($matches[2]);
                    $location = parse_url($location);
                    if($location) {
                        $trips++;
                        if(!isset($location['scheme'])) {
                            $location['scheme'] = $prefix;
                        }

                        if(!isset($location['host'])) {
                            $location['host'] = $url;
                        }

                        if(!isset($location['path'])) {
                            $location['path'] = '';
                        }

                        if(isset($location['query'])) {
                            $location['path'] .= '?' . $location['query'];
                        }

                        $returnValue = self::checkHTTP($ip, $location['host'], $returnCode, $extraData, $errorDetails, $key, $auth, $location['scheme'], $location['path'], $cookies, $trips, $attempts);
                        $extraData['redirect_count'] = $trips;
                        return $returnValue;
                    }
                }
            }
        }

        $curlErrorCode = curl_errno($handle);
        if($curlErrorCode) {
            $httpCode = $curlErrorCode;
        }

        

        if($httpCode !== 200) {
            $header_size = curl_getinfo($handle, CURLINFO_HEADER_SIZE);
            $header = substr($curlResponse, 0, $header_size);
            $body = substr($curlResponse, $header_size);
            $json = json_decode($body, true);
            if($json) {
                $errorDetails = $json;
            }
        }

        curl_close($handle);
        
        if($httpCode !== 200 && $attempts < 3) {
            $attempts++;
            return self::checkHTTP($ip, $url, $returnCode, $extraData, $errorDetails, $key, $auth, $prefix , $suffix, $cookies, $trips, $attempts);
        } else if($returnCode) {
            return $httpCode;
        } else if($httpCode === 200) {
            return true;
        }

        return false;
    }

    public static function checkClient($ip, $url, $prefix = 'http', $suffix = '', $cookies = [], $trips = 0, $attempts = 1) {
        $httpCode = false;
        $port = 80;
        if($prefix === 'https') {
            $port = 443;
        }

        if($trips >= 20) {
            return false;
        }

        $handle = curl_init();

        $tempCookiesFile = sys_get_temp_dir() . '/' . md5($url . $ip) . '.cookies';
        if(!is_file($tempCookiesFile)) {
            file_put_contents($tempCookiesFile, json_encode($cookies));
        } else if(empty($cookies)) {
            $cookies = (array)json_decode(file_get_contents($tempCookiesFile));
        }

        curl_setopt($handle, CURLOPT_URL, $prefix . '://' . $ip . $suffix);
        curl_setopt($handle, CURLOPT_FOLLOWLOCATION, false);
        curl_setopt($handle, CURLOPT_HEADER, true);
        curl_setopt($handle, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($handle, CURLOPT_HTTPHEADER, ['Host: ' . $url, 'X-Hc-Skip-App-Stats: 1']);
        curl_setopt($handle, CURLOPT_SSL_VERIFYPEER, false);
        curl_setopt($handle, CURLOPT_SSL_VERIFYHOST, false);
        curl_setopt($handle, CURLOPT_COOKIE, http_build_query($cookies));
        curl_setopt($handle, CURLOPT_CONNECTTIMEOUT, 60);
        curl_setopt($handle, CURLOPT_TIMEOUT, 60);

        $curlResponse = curl_exec($handle);
        if($curlResponse) {
            $extraData = curl_getinfo($handle);
            $httpCode = $extraData['http_code'];
            if($httpCode === 301 || $httpCode === 302) {
                $oldCookies = $cookies;
                $matches = [];
                preg_match_all('/^Set-Cookie: (.*?)=(.*?);/m', $curlResponse, $matches);
                if(isset($matches[1])) {
                    foreach($matches[1] as $index => $cookie) {
                        $cookies[$cookie] = $matches[2][$index];
                    }
                }
                if($cookies !== $oldCookies) {
                    file_put_contents($tempCookiesFile, json_encode($cookies));
                }

                $matches = [];
                $result = preg_match('/(Location:|URI:)(.*?)\n/', $curlResponse, $matches);
                if(isset($matches[2])) {
                    $location = trim($matches[2]);
                    $location = parse_url($location);
                    if($location) {
                        $trips++;
                        if(!isset($location['scheme'])) {
                            $location['scheme'] = $prefix;
                        }

                        if(!isset($location['host'])) {
                            $location['host'] = $url;
                        }

                        if(!isset($location['path'])) {
                            $location['path'] = '';
                        }

                        if(isset($location['query']) && !empty($location['query'])) {
                            $queryArr = explode('?', $location['query']);
                            $location['path'] .= '?' . $queryArr[0];
                        }
                        return self::checkClient($ip, $location['host'], $location['scheme'], $location['path'], $cookies, $trips, $attempts);
                    }
                }
            }
        }

        $curlErrorCode = curl_errno($handle);
        if($curlErrorCode) {
            $httpCode = $curlErrorCode;
        }

        $header_size = curl_getinfo($handle, CURLINFO_HEADER_SIZE);
        $header = substr($curlResponse, 0, $header_size);
        $body = substr($curlResponse, $header_size);

        curl_close($handle);
        
        if($httpCode === 200) {
            return json_decode($body, true);
        } else if($attempts < 3) {
            $attempts++;
            return self::checkClient($ip, $url, $prefix, $suffix, $cookies, $trips, $attempts);
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

        $data = array_reverse($data, 1);
        $db = new \HC\DB();
        $users = $db->read('users', ['firstName', 'lastName', 'email'], ['notify' => 1]);
        if($users) {
            $email = new \HC\Email();
            $title = $data['Server Title'] . ' - ' . $data['Domain Title'] . ': ' . 'Failed (' . $data['Code']. ' - ' . $data['Code Message'] . ')';
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
        $query = $db->write('servers', $data);
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
