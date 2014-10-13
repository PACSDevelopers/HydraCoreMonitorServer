<?hh
namespace HCPublic\Ajax\Domains\Domain;

class ProcessNewDomainAjax extends \HC\Ajax {

	protected $settings = [
			'authentication' => true
	];

	public function init($GET = [], $POST = []) {

        $response = [];
        
		// Put all errors in an array
		$response['errors'] = [];
        
		if(!isset($POST['data'])){
            $response['errors']['e1'] = true;
		}

		if(count($response['errors']) == 0){
            $insertKeys = [
                'domainTitle' => 'title',
                'domainURL' => 'url'
            ];
            
            $isValid = true;
            $data = [];
            foreach($POST['data'] as $key => $value) {
                if(isset($insertKeys[$key])) {
                    if(is_string($value)) {
                            $data[$insertKeys[$key]] = <x:frag>{$value}</x:frag>;
                    } else {
                            $data[$insertKeys[$key]] = $value;
                    }
                }
            }

            if(isset($data['url'])) {
                if(gethostbyname($data['url']) === $data['url']) {
                    $isValid = false;
                } else {
                    $httpCode = false;
                    $data['url'] = str_replace('http://', '', $data['url']);
                    $data['url'] = str_replace('https://', '', $data['url']);
                    $data['url'] = rtrim((string)$data['url'], '/');
                    $handle = curl_init('http://' . $data['url']);

                    curl_setopt($handle, CURLOPT_FOLLOWLOCATION, true);
                    $tempCookiesFile = sys_get_temp_dir() . '/' . md5((string)$data['url']) . '.cookies';
                    if(!is_file($tempCookiesFile)) {
                        touch($tempCookiesFile);
                    }
                    curl_setopt($handle, CURLOPT_RETURNTRANSFER, true);
                    curl_setopt($handle, CURLOPT_COOKIEJAR, $tempCookiesFile);
                    curl_setopt($handle, CURLOPT_COOKIEFILE, $tempCookiesFile);

                    $curlResponse = curl_exec($handle);
                    if($curlResponse) {
                        $httpCode = curl_getinfo($handle, CURLINFO_HTTP_CODE);
                    }

                    curl_close($handle);

                    if($httpCode !== 200) {
                        $isValid = false;
                    }
                }
            }
            
            if(!isset($data['createdBy'])) {
                $data['createdBy'] = $_SESSION['user']->getUserID();
            }

            if(!isset($data['dateCreated'])) {
                $data['dateCreated'] = time();
            }

            if($isValid) {
                $domain = \HCMS\Domain::create($data);

                if($domain){
                    $response = ['status' => 1, 'data' => $POST['data'], 'domainID' => $domain->id];
                } else {
                    $response['errors']['e2'] = true;
                }
            } else {
                $response['errors']['e3'] = true;
            }
		}

		$this->body = $response;
		return 1;
	}
}
