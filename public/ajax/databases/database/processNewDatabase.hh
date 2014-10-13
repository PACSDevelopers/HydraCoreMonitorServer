<?hh
namespace HCPublic\Ajax\Databases\Database;

class ProcessNewDatabaseAjax extends \HC\Ajax {

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
                'databaseTitle' => 'title',
                'databaseIP' => 'ip'
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

            if(isset($data['ip'])) {
                $data['ip'] = ip2long($data['ip']);
                if(!$data['ip']) {
                    $isValid = false;
                } else {
                    if(!$fp = @fsockopen(long2ip($data['ip']), 3306)){
                        $isValid = false;
                    } else {
                        fclose($fp);
                    }
                }
            }
            
            if(!isset($data['createdBy'])) {
                $data['createdBy'] = $_SESSION['user']->getUserID();
            }

            if(!isset($data['dateCreated'])) {
                $data['dateCreated'] = time();
            }
            
            if($isValid) {
                $database = \HCMS\Database::create($data);

                if($database){
                    $response = ['status' => 1, 'data' => $POST['data'], 'databaseID' => $database->id];
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
