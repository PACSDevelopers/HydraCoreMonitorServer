<?hh
namespace HCPublic\Ajax\Domains\Domain;

class ProcessUpdateDomainAjax extends \HC\Ajax {
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
            if(isset($POST['data']['domainID'])){
                $domain = new \HCMS\Domain(['id' => $POST['data']['domainID']]);
                if($domain->checkExists()) {
                    $response = $domain->update($POST);
                } else {
                    $response['errors']['e3'] = true;
                }
            } else {
                $response['errors']['e2'] = true;
            }
		}

		$this->body = $response;
		return 1;
	}
}
