<?hh
namespace HCPublic\Data\Templates;

class TemplatePage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Data - Templates',
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

	public function init($GET, $POST) {

        $template = new \HCMS\Template(['id' => $GET['id']]);

        if(!$template->checkExists()) {
            return 404;
        }

        $this->settings['views']['header']['pageName'] .= $template->title;
        
        if($_SESSION['user']->hasPermission('Delete')) {
            $this->settings['views']['body']['headerButtonsRight'][] = <button class="btn btn-primary pull-right" onclick="deleteTemplate();">Delete Template</button>;
        }

        $this->settings['views']['body']['headerButtonsRight'][] = <button class="btn btn-primary pull-right" onclick="importTemplate();">Import</button>;

        $isDisabled = !$_SESSION['user']->hasPermission('Edit');
        
		$this->body = <x:frag>
            <div class="container">
                <div class="row">
                    <h1>Data - Templates - Details</h1>
                    <form action="" class="form-horizontal" role="form">
                            <input type="hidden" name="templateID" id="templateID" value={$template->id} />
                            <div class="form-group">
                                    <label class="col-sm-2 control-label" for="templateTitle">Title</label>
                                    <div class="col-sm-10">
                                            <input type="text" class="form-control" placeholder="Title" id="templateTitle" required="required" maxlength="50" data-orgval={$template->title} value={$template->title} />
                                    </div>
                            </div>
                            
                            <div class="form-group">
                                    <div id="alertBox"></div>
                                    <button type="button" class="btn btn-default pull-right" onclick="updateTemplate();">Update Template</button>
                            </div>
                        </form>
                </div>
                <div id="templateModal" class="modal fade" tabindex="-1" role="dialog" aria-labelledby="Import Template" aria-hidden="true">
                            <div class="modal-dialog">
                                <div class="modal-content">
                                    <div class="modal-header">
                                        <button type="button" class="close" data-dismiss="modal"><span aria-hidden="true">&times;</span><span class="sr-only">Close</span></button>
                                        <h4 class="modal-title">Import Template</h4>
                                    </div>
                                    <div id="templateModalBody" class="modal-body">
                                    </div>
                                    <div class="modal-footer">
                                        <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
                                        <button type="button" class="btn btn-primary" id="templateModalSaveButton" disabled="disabled"> 
                                            Import
                                        </button>
                                    </div>
                                </div>
                            </div>
                        </div>
                <div class="row" id="tableList">
                    
                </div>
            </div>
        </x:frag>;
        
        return 1;
	}
};
