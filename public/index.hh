<?hh
namespace HCPublic;

class IndexPage extends \HC\Page {

	protected $settings = [
		'views' => [
			'header' => [
				'pageName' => 'Status',
				'scss' => [
					'main' => true,
					'login' => true
				],
				'js' => [
					'extenders' => true,
					'main' => true,
					'bootstrap-functions' => true,
					'forms' => true,
                    'dashboard' => true
				]
			],
			'body' => true,
			'footer' => true
		]
	];

	public function init($GET = [], $POST = []) :int {
        $this->body = <x:frag>
            <div class="container">
                    <div class="row">
                        <form class="form-signin" role="form" id="loginForm" autocomplete="on">
                                <h2 class="form-signin-heading">{SITE_NAME} - Status <a class="btn btn-sm btn-primary pull-right" href="/login">Login</a></h2>
                                <div class="clearfix"></div>
                                <select class="form-control" id="timeScale">
                                    <option value="0">Hour</option>
                                    <option value="1">Day</option>
                                    <option value="2">Week</option>
                                    <option value="3">Month</option>
                                </select>
                        </form>
                    </div>
                    <div class="row">
                        <div class="row">
                            <div id="serverHistoryApplicationResponseTime" class="chart forceGPU noselect">
                                <div class="spinner">
                                  <div class="rect1"></div>
                                  <div class="rect2"></div>
                                  <div class="rect3"></div>
                                  <div class="rect4"></div>
                                  <div class="rect5"></div>
                                </div>
                            </div>
                        </div>

                        <div class="row">
                            <div class="col-lg-12">
                                <div id="serverHistoryUsage" class="chart forceGPU noselect">
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

                        <div class="row">
                            <div class="col-lg-4">
                                <div id="serverHistoryApplicationRPM" class="chart forceGPU noselect">
                                    <div class="spinner">
                                      <div class="rect1"></div>
                                      <div class="rect2"></div>
                                      <div class="rect3"></div>
                                      <div class="rect4"></div>
                                      <div class="rect5"></div>
                                    </div>
                                </div>
                            </div>

                            <div class="col-lg-4">
                                <div id="serverHistoryApplicationQPM" class="chart forceGPU noselect">
                                    <div class="spinner">
                                      <div class="rect1"></div>
                                      <div class="rect2"></div>
                                      <div class="rect3"></div>
                                      <div class="rect4"></div>
                                      <div class="rect5"></div>
                                    </div>
                                </div>
                            </div>

                            <div class="col-lg-4">
                                <div id="serverHistoryApplicationAVGTimeCPUBound" class="chart forceGPU noselect">
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

                        <div class="row">
                            <div class="col-lg-4">
                                <div id="serverHistoryIOWait" class="chart forceGPU noselect">
                                    <div class="spinner">
                                      <div class="rect1"></div>
                                      <div class="rect2"></div>
                                      <div class="rect3"></div>
                                      <div class="rect4"></div>
                                      <div class="rect5"></div>
                                    </div>
                                </div>
                            </div>

                            <div class="col-lg-4">
                                <div id="serverHistoryTPS" class="chart forceGPU noselect">
                                    <div class="spinner">
                                      <div class="rect1"></div>
                                      <div class="rect2"></div>
                                      <div class="rect3"></div>
                                      <div class="rect4"></div>
                                      <div class="rect5"></div>
                                    </div>
                                </div>
                            </div>

                            <div class="col-lg-4">
                                <div id="serverHistoryNetworkTraffic" class="chart forceGPU noselect">
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
            </div>
        </x:frag>;

        return 1;
	}
}
