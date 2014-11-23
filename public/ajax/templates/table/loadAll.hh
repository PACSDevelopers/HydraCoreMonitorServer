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
                $db = new \HC\DB();
                $result = $db->read('data_template_tables', ['id', 'name', 'alias'], ['templateID' => $POST['data']['templateID']]);
                
                if($result){
                    foreach($result as $key => $row) {
                        $colResult = $db->read('data_template_columns', ['id', 'name', 'alias', 'relationTable', 'relationColumn'], ['templateID' => $POST['data']['templateID'], 'tableID' => $row['id']]);
                        if(!$colResult) {
                            $colResult = [];
                        }
                        $result[$key]['columns'] = $colResult;
                    }
                    
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
