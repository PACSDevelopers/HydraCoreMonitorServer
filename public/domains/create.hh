<?hh
namespace HCPublic\Domains;

class CreatePage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Domains - Create Domain',
							'scss' => [
									'main' => true
							],
							'js' => [
									'main' => true,
									'bootstrap-functions' => true,
									'forms'      => true,
									'domainForm' => true
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
                                <h1>Create Domain</h1>
                            </div>
    
                            <form action="" class="form-horizontal" role="form">
            
                                <div class="form-group">
                                        <label class="col-sm-2 control-label" for="domainTitle">Title</label>
                                        <div class="col-sm-10">
                                                <input type="text" class="form-control" placeholder="Title" id="domainTitle" required="required" maxlength="50" />
                                        </div>
                                </div>

                                <div class="form-group">
                                        <label class="col-sm-2 control-label" for="domainURL">URL</label>
                                        <div class="col-sm-10">
                                                <input type="url" class="form-control" placeholder="URL" id="domainURL" required="required" data-url-extension="false" />
                                        </div>
                                </div>
                                
                                <div class="form-group">
                                        <div id="alertBox"></div>
                                        <button type="button" class="btn btn-default pull-right" onclick="submitForm();">Create Domain</button>
                                </div>

                            </form>
                        </div>
                    </div>;
		return 1;
	}
}
