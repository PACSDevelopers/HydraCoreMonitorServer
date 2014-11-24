<?hh
namespace HCPublic\Ajax\Templates\Table;

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
            if(isset($POST['data']['tableID'])) {
                $db = new \HC\DB();
                $result1 = $db->delete('data_template_tables', ['id' => $POST['data']['tableID']]);
                $result2 = $db->delete('data_template_columns', ['tableID' => $POST['data']['tableID']]);
                
                if($result1 && $result2){
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
