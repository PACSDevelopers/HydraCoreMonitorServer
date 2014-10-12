<?hh
namespace HCPublic;

class IndexPage extends \HC\Page {

	protected $settings = [
		'views' => [
			'header' => [
				'pageName' => 'Login',
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
									<h2 class="form-signin-heading">{SITE_NAME} - Login</h2>
									<div id="alertBox"></div>
									<label for="loginEmail" class="input-group">
											<span class="input-group-addon"><span class="glyphicons envelope"></span></span>
											<input type="email" name="loginEmail" id="loginEmail" class="form-control" placeholder="Email Address" required="required" autofocus="autofocus" autocomplete="on" oninvalid="return false;" />
									</label>
									<br />
									<label for="loginPassword" class="input-group">
											<span class="input-group-addon"><span class="glyphicons keys"></span></span>
											<input type="password" name="loginPassword" id="loginPassword" class="form-control" placeholder="Password" required="required" pattern="^(?=.*\d)(?=.*[a-z])(?=.*[A-Z])(?!.*\s).*$" autocomplete="off" oninvalid="return false;" />
									</label>
									<button class="btn btn-lg btn-primary btn-block" type="submit">Login</button>
							</form>
					</div>
				</x:frag>;

				return 1;
	}
}
