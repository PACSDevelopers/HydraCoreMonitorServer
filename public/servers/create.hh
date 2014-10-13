<?hh
namespace HCPublic\Servers;

class CreatePage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Servers - Create Server',
							'scss' => [
									'main' => true
							],
							'js' => [
									'main' => true,
									'bootstrap-functions' => true,
									'forms'      => true,
									'serverForm' => true
							]
					],
					'body' => true,
					'footer' => true
			],
			'forms' => true,
			'authentication' => true
	];

	public function init($GET = [], $POST = []) {
		$db = new \HC\DB();
        
		$this->body = <div class="container">
                        <div class="row">
                            <div class="page-header">
                                <h1>Create Server</h1>
                            </div>
    
                            <form action="" class="form-horizontal" role="form">
            
                                <div class="form-group">
                                        <label class="col-sm-2 control-label" for="serverTitle">Title</label>
                                        <div class="col-sm-10">
                                                <input type="text" class="form-control" placeholder="Title" id="serverTitle" required="required" maxlength="50" />
                                        </div>
                                </div>

                                <div class="form-group">
                                        <label class="col-sm-2 control-label" for="serverIP">IP</label>
                                        <div class="col-sm-10">
                                                <input type="text" class="form-control" placeholder="IP" id="serverIP" required="required" />
                                        </div>
                                </div>
                                
                                <div class="form-group">
                                        <div id="alertBox"></div>
                                        <button type="button" class="btn btn-default pull-right" onclick="submitForm();">Create Server</button>
                                </div>

                            </form>
                        </div>
                    </div>;
		return 1;
	}
}
