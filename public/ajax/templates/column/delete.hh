<?hh
namespace HCPublic\Ajax\Templates\Column;

class DeleteAjax extends \HC\Ajax {

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
            if(isset($POST['data']['columnID'])) {
                $db = new \HC\DB();
                $result = $db->delete('data_template_columns', ['id' => $POST['data']['columnID']]);
                
                if($result){
                    $response = ['status' => 1];
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
