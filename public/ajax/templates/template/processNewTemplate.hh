<?hh
namespace HCPublic\Ajax\Templates\Template;

class ProcessNewTemplateAjax extends \HC\Ajax {

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
                'templateTitle' => 'title',
                'templateStatus' => 'status',
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
            
            if(!isset($data['createdBy'])) {
                $data['createdBy'] = $_SESSION['user']->getUserID();
            }

            if(!isset($data['dateCreated'])) {
                $data['dateCreated'] = time();
            }

            if($isValid) {
                $template = \HCMS\Template::create($data);

                if($template){
                    $response = ['status' => 1, 'data' => $POST['data'], 'templateID' => $template->id];
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
