<?hh
namespace HCPublic\Ajax\Templates\Table;

class AddAjax extends \HC\Ajax {

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
                $db = new \HC\DB();
                $result1 = $db->write('data_template_tables', ['templateID' => $POST['data']['templateID']]);
                
                if($result1){
                    $response = ['status' => 1, 'id' => $db->getLastID()];
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
