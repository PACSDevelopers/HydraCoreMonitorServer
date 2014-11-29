<?hh
namespace HCPublic\Ajax\Databases\Database;

class ProcessDatabaseStatusAjax extends \HC\Ajax {
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
            if(isset($POST['data']['databaseID'])){
                $database = new \HCMS\Database(['id' => $POST['data']['databaseID']]);
                if($database->checkExists()) {
                    $isValidConnection = false;
                    $connection = $database->getDatabaseConnection();
                    if($connection) {
                        if($connection->isActive()) {
                            $isValidConnection = true;
                            $connection->disconnect();
                        }
                    }
                    
                    $response = ['status' => $isValidConnection];
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
