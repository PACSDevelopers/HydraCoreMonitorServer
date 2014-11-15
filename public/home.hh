<?hh
namespace HCPublic;

class HomePage extends \HC\Page {
	protected $settings = [
		'views' => [
			'header' => [
				'pageName' => 'Home',
				'scss' => [
					'main' => true,
				],
				'js' => [
					'extenders' => true,
					'main' => true,
					'bootstrap-functions' => true,
                    'dashboard' => true
				]
			],
			'body' => true,
			'footer' => true,
		],
		'authentication' => true,
	];

	public function init($GET = [], $POST = []) {
		$container = <div class="container">
            <div class="row">
                <h1>{SITE_NAME} - {$this->settings['views']['header']['pageName']}</h1>
                <select class="form-control" id="timeScale">
                    <option value="0">Hour</option>
                    <option value="1">Day</option>
                    <option value="2">Week</option>
                    <option value="3">Month</option>
                </select>
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
        </div>;

		// Add the row to the container, and render the body
		$this->body = $container;

		return 1;
	}
}
