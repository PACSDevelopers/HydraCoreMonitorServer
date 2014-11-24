<?hh
namespace HCPublic\Ajax\Templates\Column;

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
            if(isset($POST['data']['tableID'])) {
                $db = new \HC\DB();
                $result = $db->write('data_template_columns', ['tableID' => $POST['data']['tableID'], 'templateID' => $POST['data']['templateID']]);
                
                if($result){
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
