<?hh
namespace HCPublic;

class UnsupportedPage extends \HC\Page {
	protected $settings = [
		'views' => [
			'header' => [
				'pageName' => 'Unsupported Browser',
				'scss' => [
					'main' => true
				],
				'js' => [
					'main' => true,
					'bootstrap-functions' => true
				]
			],
			'body' => true,
			'footer' => true
		]
	];

	public function init($GET = [], $POST = []) {

		$this->body = <div class="container" style="width: 1024px;">
				<h2 class="form-signin-heading">Unsupported Browser</h2>
			</div>;

		return 1;
	}
}
