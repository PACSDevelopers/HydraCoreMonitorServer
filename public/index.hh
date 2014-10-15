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
                        </form>
                    </div>
                    <div class="row">
                        <div class="col-lg-4">
                                <div id="domainHistoryAvailability"></div>
                                <div id="domainHistoryResponseTime"></div>
                        </div>

                        <div class="col-lg-4">
                                <div id="serverHistoryAvailability"></div>
                                <div id="serverHistoryResponseTime"></div>
                        </div>

                        <div class="col-lg-4">
                                <div id="databaseHistoryAvailability"></div>
                                <div id="databaseHistoryResponseTime"></div>
                        </div>
                    </div>
            </div>
        </x:frag>;

        return 1;
	}
}
