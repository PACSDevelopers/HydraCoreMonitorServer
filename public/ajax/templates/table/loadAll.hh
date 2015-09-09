<?hh
namespace HCPublic\Ajax\Templates\Table;

class LoadAllAjax extends \HC\Ajax {

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
            if(isset($POST['data']['templateID'])) {
                $template = new \HCMS\Template(['id' => $POST['data']['templateID']]);

                $result = $template->load();

                if($result){
                    $response = ['status' => 1, 'result' => $result];
                } else {
                    $response = ['status' => 1, 'result' => []];
                }
            } else {
                $response['errors']['e3'] = true;
            }
		}

		$this->body = $response;
		return 1;
	}
}
