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
									'domains/domainForm' => true,
                                    'domains/domainTable' => true,
                                    'domains/domainCharts' => true
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
            $this->settings['views']['body']['headerButtonsRight'] = [<button class="btn btn-primary pull-right" onclick="deleteDomain();">Delete Domain</button>];
        }

        $isDisabled = !$_SESSION['user']->hasPermission('Edit');
        
        $this->body = <x:frag>
                        <div class="container">
                            <h1>Domain Details</h1>
                            <div class="row">
                                    <form action="" class="form-horizontal" role="form"> 
                                            <input type="hidden" name="domainID" id="domainID" value={$domain->id} />
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
                                                    <label class="col-sm-2 control-label" for="domainStatus">Status</label>
    
                                                    <div class="col-sm-10">
                                                            <span class="domainStatusIcon glyphicons circle_question_mark pull-right" data-id={$domain->id}></span>
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
                            <div class="row">
                                <select class="form-control" id="timeScale">
                                    <option value="0">Hour</option>
                                    <option value="1">Day</option>
                                    <option value="2">Week</option>
                                    <option value="3">Month</option>
                                </select>
                            </div>
                            <div class="row">
                                <div class="col-lg-6">
                                    <div id="historyAvailability" class="chart forceGPU noselect">
                                        <div class="spinner">
                                          <div class="rect1"></div>
                                          <div class="rect2"></div>
                                          <div class="rect3"></div>
                                          <div class="rect4"></div>
                                          <div class="rect5"></div>
                                        </div>
                                    </div>
                                </div>
    
                                <div class="col-lg-6">
                                    <div id="historyResponseTime" class="chart forceGPU noselect">
                                        <div class="spinner">
                                          <div class="rect1"></div>
                                          <div class="rect2"></div>
                                          <div class="rect3"></div>
                                          <div class="rect4"></div>
                                          <div class="rect5"></div>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                      </x:frag>;
        
		return 1;
	}
}
