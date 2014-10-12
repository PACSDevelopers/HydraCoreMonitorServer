<?hh
namespace HCPublic;

class LogoutPage extends \HC\Page {

	protected $settings = [
		'views' => [
			'header' => [
				'pageName' => 'Logout',
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
		\HC\User::endSession();
		header_remove('Location');

        $this->body = <x:frag>
					<div class="container">
							<form class="form-signin" role="form" id="loginForm" autocomplete="on">
									<h2 class="form-signin-heading">{SITE_NAME} - Logout</h2>
									<a class="btn btn-lg btn-primary btn-block" href="/">Login Again</a>
							</form>
					</div>
				</x:frag>;

		return 1;
	}
}
