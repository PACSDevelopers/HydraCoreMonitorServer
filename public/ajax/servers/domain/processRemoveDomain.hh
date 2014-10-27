<?hh
namespace HCPublic\Ajax\Servers\Domain;

class ProcessRemoveDomainAjax extends \HC\Ajax {
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
            if(isset($POST['data']['serverID'])){
                $server = new \HCMS\Server(['id' => $POST['data']['serverID']]);
                if($server->checkExists()) {
                    $db = new \HC\DB();
                    $result = $db->delete('server_mapping', ['domainID' => $POST['data']['domainID'], 'serverID' => $POST['data']['serverID']]);
                    if($result) {
                        $response = ['status' => 1];
                    } else {
                        $response['errors']['e4'] = true;
                    }
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
