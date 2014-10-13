<?hh

namespace HCPublic\Domains;

class DomainPage extends \HC\Page {
	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Domains - ',
							'scss' => [
									'main' => true
							],
							'js' => [
									'extenders' => true,
									'main' => true,
									'bootstrap-functions' => true,
									'forms' => true,
									'domainForm' => true
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

	public function init($GET = [], $POST = []) {
		if (!isset($GET['id'])) {
			return 404;
		}

		$domain = new \HCMS\Domain(['id' => $GET['id']]);
        
        if(!$domain->checkExists()) {
            return 404;
        }
        
		$this->settings['views']['header']['pageName'] .= $domain->title;
		$db = new \HC\DB();
        
        if($_SESSION['user']->hasPermission('Delete')) {
            $this->settings['views']['body']['headerButtonsRight'] = [<button class="btn btn-primary" onclick="deleteDomain();">Delete Domain</button>];
        }

        $isDisabled = !$_SESSION['user']->hasPermission('Edit');
        
        $this->body = <x:frag>
                        <div class="row col-lg-2 col-md-0 col-sm-0">
                        </div>
                        <div class="row col-lg-8 col-md-12 col-sm-12">
                            <h1>Domain Details</h1>
                            <div class="row">
                                    <form action="" class="form-horizontal" role="form"> 
                                            <input type="hidden" name="domainD" id="domainID" value={$domain->id} />
                                            <div class="form-group">
                                                    <label class="col-sm-2 control-label" for="domainTitle">Title</label>
    
                                                    <div class="col-sm-10">
                                                            <input type="text" disabled={$isDisabled} class="form-control" placeholder="Title"
                                                                        data-orgval={$domain->title}
                                                                        id="domainTitle" required="required" maxlength="50" value={$domain->title} />
                                                    </div>
                                            </div>
    
                                            <div class="form-group">
                                                    <label class="col-sm-2 control-label" for="domainURL">URL</label>
    
                                                    <div class="col-sm-10">
                                                            <input type="url" disabled={$isDisabled} class="form-control" placeholder="URL" id="domainURL"
                                                                        data-orgval={$domain->url}
                                                                        data-url-extension="false"
                                                                        required="required" value={$domain->url} />
                                                    </div>
                                            </div>
    
                                            <div class="form-group">
                                                    <div id="alertBox"></div>
                                                    <div class="col-sm-2"></div>
                                                    <div class="col-sm-10 text-right">
                                                        <button type="button" disabled={$isDisabled} class="btn btn-primary" onclick="updateForm();">Update Domain</button>
                                                    </div>
                                            </div>
                                    </form>
                            </div>
                        </div>
                        <div class="row col-lg-2 col-md-0 col-sm-0">
                        </div>
                      </x:frag>;
        
		return 1;
	}
}
