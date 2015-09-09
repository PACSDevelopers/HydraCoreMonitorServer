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
        if(isset($_SESSION['user'])){
			header('Location: /home');
		    return 1;
        }

        $scheduledDowntime = <ul class="list-group" style="max-height: 600px; overflow-y: auto; overflow-x: hidden;"></ul>;

        $this->body = <x:frag>
            <div class="container">
                <div class="row">
                    <form class="form-signin" role="form" id="loginForm" autocomplete="on">
                            <h2 class="form-signin-heading">{SITE_NAME} - Status</h2>
                            <div class="clearfix"></div>
                    </form>
                </div>
                <div class="row">
                    <div class="col-md-4">
                        <h3 class="text-center">Domains</h3>
                        <div id="domainsUp" class="chart forceGPU noselect">
                            <div class="spinner">
                              <div class="rect1"></div>
                              <div class="rect2"></div>
                              <div class="rect3"></div>
                              <div class="rect4"></div>
                              <div class="rect5"></div>
                            </div>
                        </div>
                    </div>
                    <div class="col-md-4">
                        <h3 class="text-center">Servers</h3>
                        <div id="serversUp" class="chart forceGPU noselect">
                            <div class="spinner">
                              <div class="rect1"></div>
                              <div class="rect2"></div>
                              <div class="rect3"></div>
                              <div class="rect4"></div>
                              <div class="rect5"></div>
                            </div>
                        </div>
                    </div>
                    <div class="col-md-4">
                        <h3 class="text-center">Databases</h3>
                        <div id="databasesUp" class="chart forceGPU noselect">
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
                    <div class="col-md-6">
                        <h4 class="text-center">Statistics</h4>
                        <div class="row">
                            <select class="form-control" id="timeScale">
                                <option value="0">Hour</option>
                                <option value="1">Day</option>
                                <option value="2">Week</option>
                                <option value="3">Month</option>
                            </select>
                        </div>
                        <div class="row">
                            <div id="historyAvailability" class="chart serverSideChart forceGPU noselect">
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
                            <div id="serverHistoryApplicationResponseTime" class="chart serverSideChart forceGPU noselect">
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
                            <div id="historyResponseTime" class="chart serverSideChart forceGPU noselect">
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
                    <div class="col-md-6">
                        <h4 class="text-center">Scheduled Downtime</h4>
                        {$scheduledDowntime}
                    </div>
                </div>
            </div>
        </x:frag>;

        return 1;
	}
}
