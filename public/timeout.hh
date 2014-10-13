<?hh
namespace HCPublic;

class TimeoutPage extends \HC\Page {

	protected $settings = [
		'views' => [
			'header' => [
				'pageName' => 'Timeout',
				'scss' => [
					'main' => true,
					'login' => true
				],
				'js' => [
					'extenders' => true,
					'main' => true,
					'bootstrap-functions' => true,
					'forms' => true,
					'login' => true
				]
			],
			'body' => true,
			'footer' => true
		]
	];

	public function init($GET = [], $POST = []) :int {
				$this->body = <x:frag>
					<div class="container">
							<form class="form-signin" role="form" id="loginForm" autocomplete="on">
									<h2 class="form-signin-heading">{SITE_NAME} - Timeout</h2>
									<div id="alertBox" style="display: block;"><div class="alert alert-info alert-dismissable"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button><strong>Info: </strong>Your session has expired, please login for continued use.</div></div>
									<a class="btn btn-lg btn-primary btn-block" href="/">Login Again</a>
							</form>
					</div>
				</x:frag>;

				return 1;
	}
}
