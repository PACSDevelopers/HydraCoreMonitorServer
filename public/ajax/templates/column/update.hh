<?hh
namespace HCPublic\Ajax\Templates\Column;

class UpdateAjax extends \HC\Ajax {
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
            if(isset($POST['data']['columnID'])){
                
                $updateKeys = [
                    'tableColumnName' => 'name',
                    'tableColumnAlias' => 'alias',
                    'tableColumnRelatedTable' => 'relationTable',
                    'tableColumnRelatedColumn' => 'relationColumn',
                ];

                $data = [];
                foreach($POST['data'] as $key => $value) {
                    if(isset($updateKeys[$key])) {
                        if(is_string($value)) {
                            $data[$updateKeys[$key]] = <x:frag>{$value}</x:frag>;
                        } else {
                            $data[$updateKeys[$key]] = $value;
                        }
                    }
                }

                $db = new \HC\DB();
                $query = $db->update('data_template_columns', ['id' => $POST['data']['columnID']], $data);
                if($query){
                    $response = ['status' => 1];
                } else {
                    $response['errors']['e4'] = true;
                }
            } else {
                $response['errors']['e2'] = true;
            }
		}

		$this->body = $response;
		return 1;
	}
}
