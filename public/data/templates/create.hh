<?hh
namespace HCPublic\Data\Templates;

class CreatePage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Data - Templates - Create',
							'scss' => [
									'main' => true
							],
							'js' => [
									'extenders' => true,
									'main' => true,
									'bootstrap-functions' => true,
                                    'forms'      => true,
                                    'data/templates/templates' => true
							]
					],
					'body' => [
						'headerButtonsRight' => [],
					],
					'footer' => true
			],
			'forms' => true,
			'authentication' => true
	];

	public function init() {
        
		$this->body = <x:frag>
            <div class="container">
                <div class="row">
                    <h1>Data - Templates - Create</h1>
                    <form action="" class="form-horizontal" role="form">
                            <div class="form-group">
                                    <label class="col-sm-2 control-label" for="serverTitle">Title</label>
                                    <div class="col-sm-10">
                                            <input type="text" class="form-control" placeholder="Title" id="templateTitle" required="required" maxlength="50" />
                                    </div>
                            </div>
                            
                            <div class="form-group">
                                    <div id="alertBox"></div>
                                    <button type="button" class="btn btn-default pull-right" onclick="createTemplate();">Create Template</button>
                            </div>
                        </form>
                </div>
            </div>
        </x:frag>;
        
        return 1;
	}
};
