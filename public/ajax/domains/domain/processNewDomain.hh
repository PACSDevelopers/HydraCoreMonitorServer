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
                $httpCheck = \HCMS\Domain::checkHTTP($data['url']);
                if(!$httpCheck) {
                    $isValid = false;
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
