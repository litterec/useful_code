<?php

/*
Software Name: RaSolo wishcompare
Software URI: http://design.ra-solo.ru
Description: This code makes possible the Woocommerce product comparision 
Version: 1.1
Author: Andrew Galagan
Author URI: http://galagan.ra-solo.ru
License: GPL2
*/
/*  Copyright YEAR  PLUGIN_AUTHOR_NAME  (email : eastern@ukr.net)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License, version 2, as 
    published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


if ( ! defined( 'ABSPATH' ) ){
	exit;
};

if ( ! is_admin() ) add_action( 'wp_enqueue_scripts', 'rasolo_enqueue_site_scripts' );
function rasolo_enqueue_site_scripts()
		{
global $post;

//myvar_dump( $post->ID,'$post->ID');
//			exit;

$wish_compare_pages_ids=array(6731, 550, 6 );

if ( ! empty( $post->ID ) ) {
	$pst_id = $post->ID;
} else {
	$pst_id = false;
};

//myvar_dump($pst_id,'$pst_id_2341235152_');

$wish_compare_page = false;
if( is_main_rasolo_page()){
	$wish_compare_page = true;
}

if ( $pst_id && in_array( $post->ID, $wish_compare_pages_ids ) ) {
	$wish_compare_page = true;
};

if ( is_product() ) {
	$wish_compare_page = true;
} else if ( is_shop() || is_product_category() ) {
	$wish_compare_page = true;
};

if ( $wish_compare_page ) {
	wp_enqueue_script( 'rasolo-wish-compare', this_theme_url() . 'js/class-rasolo-wish-compare.js' );
	wp_enqueue_script( 'rasolo-site-tricks', this_theme_url() . 'js/site_tricks.js' );
	wp_enqueue_script( 'rasolo-wish-tricks', this_theme_url() . 'js/wish_tricks.js' );
	wp_enqueue_script( 'rasolo-cmpr-tricks', this_theme_url() . 'js/cmpr_tricks.js' );

	$page_type = 'other';
	if ( is_product() ) {
		$page_type = 'product';
	} else if ( 6731==$pst_id || is_shop() || is_product_category() ) {
		$page_type = 'shop';
	};

	wp_localize_script( 'rasolo-site-tricks', 'rasolo_params', array(
		'theme_url' => this_theme_url(),
		'wish_max' => RasoloWishCompare::$WISH_MAX_SIZE,
		'cmpr_max' => RasoloWishCompare::$CMPR_MAX_SIZE,
		'cookie_key' => RasoloWishCompare::$COOKIE_KEY,
		'this_user' => get_current_user_id(),
		'page_type' => $page_type
	) );

	wp_enqueue_style( 'rasolo-wish-compare', this_theme_url() . 'css/rasolo-wish-compare.css' );

};

if ( isset( $_GET[ 'trade_status' ] ) && in_array( $_GET[ 'trade_status' ], array( 'wholesale', 'retail' ) ) ) {
	if ( $pst_id == 12 ) wp_enqueue_script( 'rasolo-reg-tricks', this_theme_url() . '/js/register_tricks.js' );
};

wp_enqueue_style( 'font-awesome-style', this_theme_url() . 'css/font-awesome.min.css' );
wp_enqueue_style( 'font-electro-style', this_theme_url() . 'css/font-electro.css' );

		}; // The end of rasolo_enqueue_site_scripts


if ( !is_admin() ) {
	add_action( 'init', 'rasolo_init', 8 );
};

; // The end of rasolo_admin_init

function rasolo_init()
	{
rasolo_read_user_options();

//myvar_dump($rasolo_admin_options,'$rasolo_admin_options');
//die('$rasolo_admin_options');
	}; // The end of rasolo_init


add_shortcode('rasolo_compare','show_my_cmpritems');   // electro_compare_page
add_shortcode('rasolo_wishlist','show_my_wishitems'); // <!-- yith_wcwl_wishlist -->

function rasolo_woo_in_cart( $product_id )
		{
global $woocommerce;

$cart_items = $woocommerce->cart->get_cart();


foreach ( $cart_items as $key => $val ) {

	if ( $product_id == $val[ 'product_id' ] ) {
		return true;
	};
};

return false;
		}; //The end of rasolo_woo_in_cart

//add_action( 'woocommerce_before_add_to_cart_button', 'rasolo_compare_product_anchor' ); // Unknown
//add_action( 'woocommerce_after_main_content', 'rasolo_compare_product_anchor' );  // BAD! It's on the full product page
//add_action( 'woocommerce_after_shop_loop_item_title', 'rasolo_compare_product_anchor' );  // BAD!  It's in the related products inside the anchor
//add_action( 'woocommerce_after_single_product', 'rasolo_compare_product_anchor' );  // BAD!  It's in the related products inside the anchor

//add_action( 'woocommerce_after_shop_loop_item', 'rasolo_compare_product_anchor' ); // BAD! It's inside the "flex" on the mainpage
add_action( 'woocommerce_single_product_summary', 'rasolo_compare_product_anchor' );
add_action( 'electro_product_item_hover_area', 'rasolo_compare_product_anchor' );
function rasolo_compare_product_anchor()
		{

//echo '<div class="clear"></div>'.chr(10);
//echo '</div><!-- endofprevious--><div class="wish_compare_envelope">rasolo_compare_product_anchor<!-- endofcurrentcontent -->' . chr( 10 );
if ( !is_product() ) {
	echo '<!-- missed /div for not a product --></div>' . chr( 10 );
};


//global $this_user_meta;
//myvar_dump($this_user_meta,'$this_user_meta_334',1);
global $product, $wishcmpr_user_data;
$prd_id = $product->get_id();


	// <div class="action-buttons">
?>

<div class="rasolo_wishcompare_block">

	<div>

		<?php if ( ! $wishcmpr_user_data->in_wish_list( $prd_id ) ) { ?>

			<div class="rasolo_add_to_wishlist_block">
				<a
					href="#"
					rel="nofollow" data-product-id="<?php echo $prd_id; ?>"
					class="put_to_wishlist wish_compare_product_control">
					Пожелать
				</a>

				<img src="<?php echo this_theme_url(); ?>img/wpspin_light.gif"
				     class="ajax-loading" alt="loading"
				     style="visibility:hidden" width="16" height="16">
			</div>

			<div class="rasolo_added_browse_wishlist_block">
				<div class="result_block">
					<div class="wish_ajax_result operation_failed wish_compare_product_control"></div>
				</div>
				<div class="new_action_block">
					<a href="/wishlist/" rel="nofollow"
					   class="view_wish_list new_action_block wish_compare_product_control">
						Желания
					</a>
				</div>
			</div>

		<?php } else { ?>

			<div class="rasolo_browse_wishlist_block">
				<a class="view_wish_list wish_compare_product_control" href="/wishlist/" rel="nofollow">
					Желания
				</a>
			</div>


		<?php }; ?>

	</div>
	<div>

		<?php if ( ! $wishcmpr_user_data->in_cmpr_list( $prd_id ) ) { ?>


			<div class="rasolo_add_to_compare_block">
				<a href="#"
				   class="add_to_cmprlist wish_compare_product_control" data-product_id="<?php echo $prd_id; ?>">
					Сравнить
				</a>
				<img src="<?php echo this_theme_url(); ?>img/wpspin_light.gif"
				     class="ajax-loading" alt="loading"
				     style="visibility:hidden" width="16" height="16">
			</div>

			<div class="rasolo_added_browse_cmprlist_block">
				<div class="result_block">
					<div class="cmpr_ajax_result wish_compare_product_control"></div>
				</div>
				<div class="new_action_block">
					<a href="/compare/" rel="nofollow"
					   class="view_cmpr_list new_action_block wish_compare_product_control">
						Сравнения
					</a>
				</div>
			</div>



		<?php
		} else {

// view_cmpr_list
			?>

			<div class="rasolo_browse_compare_block">
				<a class="view_cmpr_list wish_compare_product_control" href="/compare/"
	   rel="nofollow">
					Сравнения
				</a>

			</div>
		<?php }; ?>

	</div>

<?php
// </div>
if(is_product()){
	echo '<!-- missed /div for product --></div>'.chr(10);
//} else {
//	echo '<!-- this is not a product -->' . chr( 10 );
};
		}; // The end of rasolo_compare_product_anchor


function rasolo_read_user_options()
	{
global $wishcmpr_user_data;
//global $this_user_meta;

$wishcmpr_user_data = New RasoloWishCompare();

//            myvar_dump($wishlist_local,'$wishlist_local 5347327');
//            myvar_dump($this,'$this 5347327');
//myvar_dump($_COOKIE,'$_COOKIE 5347327');
//                    myvar_dump($string_for_cookie,'$list_of_cmpr_id 5347327');


//$test_data_01=RasoloWishCompare::$WISH_MAX_SIZE;
//$test_data_02=RasoloWishCompare::$CMPR_MAX_SIZE;
//$test_data1=array(12,15,'ffdg',10);
//$test_data2=array(112,115,114);
//$wishcmpr_user_data->write_cmpr($test_data2);

//$wishcmpr_user_data->write_wish($test_data1);
//$test_res_01=$wishcmpr_user_data->get_error_msg();

//myvar_dump($wishcmpr_user_data,'$wishcmpr_user_data 5347327');


//die('let us write cookie');


//$test_res_02=$wishcmpr_user_data->get_error_msg();
//if($err_msg=$wishcmpr_user_data->get_error_msg()){
//    echo '<h2>Errr'.$err_msg.'</h2>';
//};


//myvar_dump($test_res_01,'$test_res_01');
//myvar_dump($test_res_02,'$test_res_02');


//$rasolo_whsh_ids=$wishcmpr_user_data->get_wish();
//if($err_msg=$wishcmpr_user_data->get_error_msg()){
//    echo '<h2>'.$err_msg.'</h2>';
//};


//$rasolo_cmpr_ids=$wishcmpr_user_data->get_cmpr();
//if($err_msg=$wishcmpr_user_data->get_error_msg()){
//    echo '<h2>'.$err_msg.'</h2>';
//};

//myvar_dump($rasolo_whsh_ids,'$rasolo_whsh_ids');
//myvar_dump($rasolo_cmpr_ids,'$rasolo_cmpr_ids');


	/*


	if($this_user=get_current_user_id()){

		$usr_meta_ser=get_user_meta( $this_user, RasoloWishCompare::$META_KEY, true );
		$this_user_meta=@unserialize($usr_meta_ser);

	//    myvar_dump($this_user_meta,'$this_user_meta _4634783_');
	//    die('$this_user_met');

		if(!is_array($this_user_meta)){
			$this_user_meta= array('wish_ids'=>array(),
						  'cmpr_ids'=>array(),
					);
			return false;
		};


	} else if(isset($_COOKIE[RasoloWishCompare::$COOKIE_KEY])){
	//    echo '$_COOKIE[OURPretty_COOKIE_KEY]='.$_COOKIE[RasoloWishCompare::$COOKIE_KEY].'{==';
		$this_user_meta=array();

		$data_url_decoded=stripslashes($_COOKIE[RasoloWishCompare::$COOKIE_KEY]);
	//    myvar_dump($data_url_decoded,'$data_url_decoded');
		$data_from_cookies=@json_decode($data_url_decoded);
		$data_from_cookies=(array)$data_from_cookies;
	//    myvar_dump($data_from_cookies,'$data_from_cookies');
		if(is_array($data_from_cookies)){
			if(isset($data_from_cookies['wish_ids']) && is_array($data_from_cookies['wish_ids'])){
				$this_wish_ids=array_filter($data_from_cookies['wish_ids'], 'is_numeric');
				$this_user_meta['wish_ids']=array_map('intval',$this_wish_ids);
			} else {
				$this_user_meta['wish_ids']=array();
			};
			if(isset($data_from_cookies['cmpr_ids']) && is_array($data_from_cookies['cmpr_ids'])){
				$this_wish_ids=array_filter($data_from_cookies['cmpr_ids'], 'is_numeric');
				$this_user_meta['cmpr_ids']=array_map('intval',$this_wish_ids);
			} else {
				$this_user_meta['cmpr_ids']=array();
			};
		} else {
			$this_user_meta = array('wish_ids'=>array(),'cmpr_ids'=>array());
		};
	} else {
		$this_user_meta = array('wish_ids'=>array(),'cmpr_ids'=>array());
	};
	*/
return true;
		}; // The end of rasolo_read_wp_options


function is_polytype_equal( $first, $second )
		{
	if ( is_bool( $first ) ) {
		return false;
	} else if ( is_string( $first ) ) {
		if ( $first == strval( $second ) ) {
			return true;
		};
	} else if ( is_numeric( $first ) ) {
		if ( intval( $first ) == intval( $second ) ) {
			return true;
		};
	} else if ( is_float( $first ) ) {
		if ( $first < 0.001 ) {
			return false;
		};
		if ( abs( $first - $second ) / $first < 0.01 ) {
			return true;
		};
	};
		}; // The end of is_polytype_equal

function show_my_cmpritems()
		{
// This function displays the products of particular category or of the archive category
// $prod_type: The selection of type data to be displayed
	global $this_user_meta, $wishcmpr_user_data;

	$cmpr_fields = array();
	$length_limit = array();
	$cmpr_fields[ 'cmpr_product' ] = 'Продукт';
	$length_limit[ 'cmpr_title' ] = 580;
//$cmpr_fields['regular_price']='Цена розничная';
	$cmpr_fields[ 'usual_price' ] = 'Цена текущая';
	$cmpr_fields[ 'description' ] = 'Описание';
	$length_limit[ 'description' ] = 540;
	$cmpr_fields[ 'shrt_dscr' ] = 'Краткое описание';
	$length_limit[ 'shrt_dscr' ] = 580;
	$cmpr_fields[ 'sale_price' ] = 'Цена распродажи';
	$cmpr_fields[ 'cats_dscr' ] = 'Категория';
	$cmpr_fields[ 'checkfor' ] = 'Наличие';
	$cmpr_fields[ 'sku' ] = 'Артикул';
	$cmpr_fields[ 'color' ] = 'Цвет';
	$cmpr_fields[ 'dims' ] = 'Размеры';
	$cmpr_fields[ 'weight' ] = 'Вес';

	$mandatory_fields=array( 'usual_price', 'shrt_dscr');

	$soft_comparison = array(); // We display this comparison line when at least one item is exist
	$soft_comparison[ ] = 'checkfor';
//global $this_user_meta;
//myvar_dump($this_user_meta,'$this_user_meta');

	/*
	$cmpr_on_the_begin=$wishcmpr_user_data->get_cmpr();
	myvar_dump($cmpr_on_the_begin,'My cmpres!');
	$remove_cmpr_rc=$wishcmpr_user_data->remove_cmpr(173);
	myvar_dump($remove_cmpr_rc,'$remove_cmpr_rc');
	$remove_cmpr_err_code=$wishcmpr_user_data->get_error_code();
	myvar_dump($remove_cmpr_err_code,'$remove_cmpr_err_code');
	$remove_cmpr_err_msg=$wishcmpr_user_data->get_error_msg();
	myvar_dump($remove_cmpr_err_msg,'$remove_cmpr_err_msg');
	$wishcmpr_user_data->write_cmpr();
	$cmpr_on_the_end=$wishcmpr_user_data->get_cmpr();
	myvar_dump($cmpr_on_the_end,'$cmpr_on_the_end');
	*/
	if(is_object( $wishcmpr_user_data)){
		$prod_type_local = $wishcmpr_user_data->get_cmpr();
	} else {
		echo '<p>Error # 3246. Incorrect data type.<p>';
		exit;
	};
//myvar_dump($prod_type_local,'$this_user_meta_423342__');

	if ( empty( $prod_type_local ) ) {
		$prod_type_local = false;
	};

	if ( $prod_type_local === false ) {
		?>


		<script>
			jQuery(document).ready(function ($) {
				$('#cmprlist_place').after(cmpr_block);
				$('#cmprlist_place').remove();
			});
		</script>
		<div id="cmprlist_place"></div>
		<?php
		return false;
	};

	$orderby = 'title'; // or 'title' date
	$order = 'ASC'; // ASC DESC


	if ( false ) {
		$prod_type_local = array( 177, 176, 175, 174 ); //
//        $prod_type_local=array(); //
	}
	$prod_type_local = array_unique( $prod_type_local );


	$query_args = array( 'posts_per_page' => - 1,
		'order' => $order,
		'orderby' => $orderby,
		'no_found_rows' => 1,
		'post_status' => 'publish',
		'post_type' => array( 'product', 'product_variation' ),
		'post__in' => $prod_type_local

	);
	$my_products = new WP_Query( $query_args );


	$howmany_products = $my_products->post_count;
//    myvar_dump($howmany_products,'$howmany_products');
//  myvar_dump($my_products,'$my_products show_my_cmpritems');


// =======================================
	if ( false && $howmany_products < 1 ) {
		?>
		<script>
			jQuery(document).ready(function ($) {
				$('#cmprlist_place_2').after(cmpr_block);
				$('#cmprlist_place_2').remove();
				$('#no_wish>div').first().html('К сожалению, в настоящее время нет возможности сравнивать товары');
			});
		</script>
		<div id="cmprlist_place_2"></div>
		<?php
		return false;
	};
	if ( false ) {

		?><div class="cmprlist">
		<script id="replacing_script_3">
			jQuery(document).ready(function ($) {
				$('#cmprlist_place_3').after(cmpr_header_block);
				$('#cmprlist_place_3').remove();
				$('#no_wish').remove();
				$('#replacing_script_3').remove();
			});
		</script>
		<div id="cmprlist_place_3"></div>
	<?php
	}
; // =======================================

	$product_dscr_tmpl = <<<PRDCT_HTML
<div class="cmpr_photo_block">
    <div class="cmpr_photo">
        <a title="Убрать товар из списка" class="close_cmpr_item" href="#">
            <i class="fa fa-times remove_from_cmprlist"></i>
        </a>
        <a href="@@@img_full@@@"><img src="@@@img_thmb@@@"></a>
    </div>
    <div class="cmpr_title">
        <a txt-storage="@@@prd_title@@@" href="@@@prd_href@@@">@@@prd_title@@@</a>
    </div>
</div>
PRDCT_HTML;

	$product_output = array();
	while ( $my_products->have_posts() ) : $my_products->the_post();

		$theid = get_the_ID();

//    myvar_dump($theid,'$theid');
//    $single_product = new WC_Product($theid);
//    $single_product = get_post($theid);
		$single_product = wc_get_product( $theid );
//    $product_post=$single_product->post;
		// its a variable product
//    if( get_post_type() <> 'product_variation' ){

		if ( get_post_type() == 'product' ) {

// 1. The basic data

			$product_post = $single_product->get_data();
//        myvar_dump($product_post,'$product_post');


            $attributes = $single_product->get_attributes();
            foreach($attributes as $attr_key=>$attr_deets){
                if('pa_postavshhiki'==$attr_key){
                    continue;
                };

                $attribute_label = wc_attribute_label($attr_key);
                $attr_value = $single_product->get_attribute( $attr_key );

                $cmpr_fields[$attr_key]=$attribute_label;
                $product_output[ $attr_key ][ $theid ]=$attr_value;

                if('pa_brand'==$attr_key && mb_strlen($attribute_label,'utf-8')>5){
                    $cmpr_fields[$attr_key]=mb_substr($attribute_label,0,5,'utf-8')
                        .'-<br>'.mb_substr($attribute_label,5,1000,'utf-8');;
                };

//                myvar_dump($attr_key ,'$attr_key ');
//                myvar_dump($attribute_label ,'$attribute_label ');
//                myvar_dump($attr_value ,'$attr_value ');

//    myvar_dump($attr_deets ,'$attr_deets ');

            };

//            myvar_dump($attributes,'$attributes ');

			//        $thumb_img_url=get_the_post_thumbnail($product_post->ID);  // This will give you the  full <img> tag

			$this_post_thumbnail_id = get_post_thumbnail_id( $theid );
			$thumb_img_url = wp_get_attachment_image_src( $this_post_thumbnail_id, 'thumbnail' );

			if ( ! empty( $thumb_img_url[ 0 ] ) ) {
				$product_output[ 'thumb_img_url' ][ $theid ] = $thumb_img_url[ 0 ];
			} else {
				$product_output[ 'thumb_img_url' ][ $theid ] = false;
			};

//        $thumb_img_url = wp_get_attachment_image_src($this_post_thumbnail_id, 'medium');
			$full_img_url = wp_get_attachment_image_src( $this_post_thumbnail_id, 'full' );

			if ( ! empty( $full_img_url[ 0 ] ) ) {
				$product_output[ 'full_img_url' ][ $theid ] = $full_img_url[ 0 ];
			} else {
				$product_output[ 'full_img_url' ][ $theid ] = false;
			};

			$product_url = '#';
			if ( ! empty( $product_post[ 'slug' ] ) ) {
				$product_url = '/product/' . $product_post[ 'slug' ];
			};


			$cats_dscr = '';
			if ( count( $product_post[ 'category_ids' ] ) ) {
				foreach ( $product_post[ 'category_ids' ] as $nth_cat_id ) {
					$cat_instance = get_term_by( 'id', $nth_cat_id, 'product_cat' );
//                myvar_dump($cat_instance,'$cat_instance');
					$cats_dscr .= ' <a href="/product-category/' . $cat_instance->slug . '">'
						. $cat_instance->name . '</a>' . chr( 10 );
				};
			};
			$cats_dscr = trim( $cats_dscr );
			if ( empty( $cats_dscr ) ) {
				$cats_dscr = 'Нет категорий';
			};
			$product_output[ 'cats_dscr' ][ $theid ] = $cats_dscr;
			//    myvar_dump($prod_img_url,'$prod_img_url');
			//    myvar_dump($thumb_img_url,'$thumb_img_url');

			$nophoto_addr = this_theme_url() . 'img/no_photo.jpg';

			if ( ! empty( $full_img_url[ 0 ] ) && ! empty( $thumb_img_url[ 0 ] ) ) {
				$img_html = '<a href="' . $full_img_url[ 0 ] . '"><img src="' . $thumb_img_url[ 0 ] . '"></a>' . chr( 10 );
			} else {
				$img_html = '<img src="' . $nophoto_addr . '">' . chr( 10 );
			};
			$product_output[ 'img_html' ][ $theid ] = $img_html;


//        if(mb_strlen($product_post['name'],'utf8')>24){
//            $small_font_wedge=' class="long_prod_name"';
//        } else {
//            $small_font_wedge='';
//        };


			if ( empty( $product_post[ 'name' ] ) ) {
				$post_ttl_dequoted = 'Нет названия';
			} else if ( strpos( '"', $product_post[ 'name' ] ) !== false ) {
				$post_ttl_dequoted = htmlentities( $product_post[ 'name' ], ENT_QUOTES, 'UTF-8' );
			} else {
				$post_ttl_dequoted = $product_post[ 'name' ];
			};
			$post_ttl_dequoted = strip_tags( $post_ttl_dequoted );
			if ( mb_strlen( $post_ttl_dequoted, 'utf-8' ) > 223 ) {
				$post_ttl_dequoted = mb_substr( $post_ttl_dequoted, 0, 220, 'utf-8' ) . '&hellip;';
			};
//        $product_output['post_ttl'][$theid]=limit_str($post_ttl_dequoted,$length_limit['cmpr_title']);
//        $product_output['post_ttl'][$theid]=$post_ttl_dequoted;  // This is unnecessary


			if ( empty( $product_post[ 'description' ] ) ) {
				$post_cnt_dequoted = 'Нет описания';
			} else if ( strpos( '"', $product_post[ 'description' ] ) !== false ) {
				$post_cnt_dequoted = htmlentities( $product_post[ 'description' ], ENT_QUOTES, 'UTF-8' );
			} else {
				$post_cnt_dequoted = $product_post[ 'description' ];
			};
			$post_excerpt_dequoted = mb_substr( strip_tags( $post_cnt_dequoted ), 0, 300, 'utf-8' );
			$product_output[ 'description' ][ $theid ] = $post_excerpt_dequoted;

			$prod_shrt_dscr = $single_product->get_short_description();
			if ( ! empty( $prod_shrt_dscr ) ) {
				$product_output[ 'shrt_dscr' ][ $theid ] = $prod_shrt_dscr;
			};

//        $product_output['regular_price'][$theid]=floatval($single_product->get_price());
			$product_output[ 'usual_price' ][ $theid ] = floatval( $single_product->get_regular_price() );
			if ( $single_product->is_on_sale() ) {
				$product_output[ 'sale_price' ][ $theid ] = floatval( $single_product->get_sale_price() );
			} else {
				$product_output[ 'sale_price' ][ $theid ] = false;
			};

			$product_output[ 'this_in_cart' ][ $theid ] = rasolo_woo_in_cart( $theid );


			$stock_quantity = $single_product->get_stock_quantity();
			if ( ! empty( $stock_quantity ) ) {
				$product_output[ 'checkfor' ][ $theid ] = $stock_quantity;
			} else {
				if ( in_array( 'checkfor', $soft_comparison ) ) {
					$product_output[ 'checkfor' ][ $theid ] = 0;
				};
			};

			$prod_sku = $single_product->get_sku();
			if ( ! empty( $prod_sku ) ) {
				$product_output[ 'sku' ][ $theid ] = $prod_sku;
			};

			$prod_width = $single_product->get_width();
			$prod_length = $single_product->get_length();
			$prod_height = $single_product->get_height();
			if ( ! empty( $prod_width ) && ! empty( $prod_length ) && ! empty( $prod_height ) ) {
				$product_output[ 'dims' ][ $theid ] = $prod_width . ' x ' . $prod_length . ' x ' . $prod_height;
			};

			$prod_weight = $single_product->get_weight();
			if ( ! empty( $prod_weight ) ) {
				$product_output[ 'weight' ][ $theid ] = $prod_weight;
			};

			$prod_attrs = get_post_meta( $theid );
//        $prod_attrs=$single_product->get_attributes();
//        myvar_dump($prod_attrs,'$prod_attrs');
			if ( isset( $prod_attrs[ '_product_attributes' ] ) ) {
				foreach ( $prod_attrs[ '_product_attributes' ] as $nth_attr ) {
					$prod_attrs_unser = @unserialize( $nth_attr );
//                myvar_dump($prod_attrs_unser,'$prod_attrs_unser');
					if ( is_array( $prod_attrs_unser ) ) {
						foreach ( $prod_attrs_unser as $nth_prod_attr_key => $nth_prod_attr_arr ) {
							if ( ! empty( $nth_prod_attr_arr[ 'name' ] ) && ! empty( $nth_prod_attr_arr[ 'value' ] ) ) {
								$product_output[ $nth_prod_attr_key ][ $theid ] = $nth_prod_attr_arr[ 'value' ];
								$cmpr_fields[ $nth_prod_attr_key ] = $nth_prod_attr_arr[ 'name' ];
							};
						};
					};
				};
			};

			$this_in_cart = rasolo_woo_in_cart( $theid );
			$product_output[ 'cart' ][ $theid ] = '<div title="' . ( $this_in_cart ?
					'Нажмите на кнопку, если хотите добавить еще один экземпляр товара в корзину' :
					'В корзине еще нет ни одного экземляра этого товара' ) .
				'">' . chr( 10 ) . '<span>' . chr( 10 ) . '<a href="' . this_request_url() . '?add-to-cart=' . $theid . '">' .
				( $this_in_cart ? 'Товар уже в корзине' : 'Добавить в корзину' ) .
				'</a>' . chr( 10 ) . '</span>' . chr( 10 ) . '</div>' . chr( 10 );

			/*

			<div title="<?php
		echo ($this_in_cart?
			  'Нажмите на кнопку, если хотите добавить еще один экземпляр товара в корзину':
			  'В корзине ещ нет ни одного экземляра этого товара');
				?>"><span><a href="/cmprlist/?add-to-cart=<?php echo $theid;
		?>"><?php
		echo ($this_in_cart?'Товар уже в корзине':'Добавить в корзину');
				?></a></span>
	</div>

			*/

//foreach($prod_attrs as $attr_key=>


// 2. The derivative data
//['product']='Продукт';
//['regular_price']='Цена розничная';
//['usual_price']='Цена текущая';
//['sale_price']='Цена распродажи';
//['cats_dscr']='Категория';
//['checkfor']='Наличие';
//['color']='Наличие';
//['add_to_cart']='Добавить';
//['sku']='Артикул';
//['weight']='Вес';
//['dims']='Размеры';
//    <div class="cmpr_photo"><a href="@@@img_full@@@"><img src="@@@img_thmb@@@"></div>
//    <div class="cmpr_title"><a href="@@@prd_href@@@">@@@prd_title@@@</a></div>


			$prd_tmp1 = str_replace( '@@@img_full@@@',
				$product_output[ 'full_img_url' ][ $theid ], $product_dscr_tmpl );
			$prd_tmp2 = str_replace( '@@@img_thmb@@@',
				$product_output[ 'thumb_img_url' ][ $theid ], $prd_tmp1 );
			$prd_tmp3 = str_replace( '@@@prd_href@@@', $product_url, $prd_tmp2 );
			$product_output[ 'cmpr_product' ][ $theid ] = str_replace( '@@@prd_title@@@',
				limit_str( $post_ttl_dequoted, $length_limit[ 'cmpr_title' ] ), $prd_tmp3 );


		};


	endwhile;
//myvar_dump($product_output,'$product_output_324634___');

	$cmpr_fields[ 'cart' ] = '&nbsp;';

	?>
<div id="cmpr_container" class="cmpr_item">

	<?php

	foreach ( $cmpr_fields as $linekey => $lineval ) {
		if ( isset( $product_output[ $linekey ] ) ) {
			if ( !in_array($linekey, $mandatory_fields) && count( array_unique( $product_output[ $linekey ] ) ) < count( $prod_type_local ) ) {
				continue;
			};
			$product_line_to_show = array();
			foreach ( $product_output[ $linekey ] as $nth_linekey => $nth_value_to_show ) {
				$product_line_to_show[ $nth_linekey ] = $nth_value_to_show;
				if ( ! empty( $length_limit[ $linekey ] ) ) {
					if ( mb_strlen( $nth_value_to_show, 'utf-8' ) > $length_limit[ $linekey ] ) {
						$product_line_to_show[ $nth_linekey ] = limit_str( $nth_value_to_show, $length_limit[ $linekey ] );
					};
				};
//            $prodid_wedge='';
//            if($linekey=='cmpr_product'){
				$prodid_wedge = ' prodid="' . $nth_linekey . '"';
//            };

				$txt_storage_wdg = ' rasolo_param="nothing"';
				if ( $linekey == 'description' || $linekey == 'shrt_dscr' ) {
					$txt_storage_wdg = ' txt-storage="' . $product_line_to_show[ $nth_linekey ] . '"';
				}
				$product_line_to_show[ $nth_linekey ] = '<div' . $txt_storage_wdg . $prodid_wedge . '>' .
					$product_line_to_show[ $nth_linekey ] . '</div>' . chr( 10 );
			};

//  prodid="'.$linekey.'"

//        myvar_dump($product_output[$linekey],'$product_output[$linekey]');
			$prods_line_cnt = '<div class="prodline" rasolo_key="'. $linekey.'">' . chr( 10 ) .
				'<div class="prodhead"><div>' . $lineval . '</div>' . chr( 10 ) .
				'</div><!-- .prod_head -->' . chr( 10 ) .
				'<div class="prodbody"><div>' . chr( 10 ) .
				'<div>' . chr( 10 ) .
				implode( '<!-- .prod_elm -->' . chr( 10 ), $product_line_to_show ) .
				'<!-- .prod_elm last -->' . chr( 10 ) . '</div>' . chr( 10 ) .
				'</div>' . chr( 10 ) .
				'</div><!-- .prodbody -->' . chr( 10 ) .
				'</div><!-- .prodline -->' . chr( 10 );
			echo $prods_line_cnt;
		};
	}
;

	/*
foreach($product_output as $nth_key=>$nth_line_spec){
	echo '<p>The item key <b>'.$nth_key.'</b>: '.chr(10);
	$whole_line_arr=array();
	$is_equal=true;
	$prev_val=false;
	$first_not_boolean=false;
	foreach($nth_line_spec as $nth_val){
		if(!$first_not_boolean){
			$prev_val=$nth_val;
		} else {
			if(!is_polytype_equal($prev_val,$nth_val)){
				$is_equal=false;
			};
		};

//        if(is_array($nth_val)){
//             myvar_dump($nth_val,'$nth_val');
//        };

		$whole_line_arr[]=$nth_val;
		$first_not_boolean=true;
	};
	echo ($is_equal?'<b>(All equal!)</b> ':'').implode(', ',$whole_line_arr).'. ';
	echo '</p>'.chr(10);
};
*/
	?>
</div>
	<?php

	wp_reset_query();

		}; // The end of show_my_cmpritems

function show_my_wishitems()
{
// This function displays the products of particular category or of the archive category
// $prod_type: The selection of type data to be displayed
	global $this_user_meta, $wishcmpr_user_data;
//global $this_user_meta;

	/*
	$wish_on_the_begin=$wishcmpr_user_data->get_wish();
	myvar_dump($wish_on_the_begin,'My wishes!');
	$remove_wish_rc=$wishcmpr_user_data->remove_wish(173);
	myvar_dump($remove_wish_rc,'$remove_wish_rc');
	$remove_wish_err_code=$wishcmpr_user_data->get_error_code();
	myvar_dump($remove_wish_err_code,'$remove_wish_err_code');
	$remove_wish_err_msg=$wishcmpr_user_data->get_error_msg();
	myvar_dump($remove_wish_err_msg,'$remove_wish_err_msg');
	$wishcmpr_user_data->write_wish();
	$wish_on_the_end=$wishcmpr_user_data->get_wish();
	myvar_dump($wish_on_the_end,'$wish_on_the_end');
	*/

		$prod_type_local = $wishcmpr_user_data->get_wish();
//        myvar_dump($wishcmpr_user_data,'$wishcmpr_user_data_32423__');
//        myvar_dump($prod_type_local,'$this_user_meta_32423__');

		/*
		$prod_type_local=$wishcmpr_user_data->get_wish();
		myvar_dump($prod_type_local,'$this_user_meta_32423__');

//         die('We have rearrange this');
//        $our_meta_ser=get_user_meta( $this_user, RasoloWishCompare::$META_KEY, true );
//        $our_meta=@unserialize($our_meta_ser);


		if(!is_array($prod_type_local) ){
			$prod_type_local = false;
		};

	} else {
//        myvar_dump($_COOKIE,'$_COOKIE');

//        $prod_type_local = $this_user_meta['wish_ids'];

	};
	*/

//	myvar_dump( $prod_type_local, '$prod_type_local2' );
//	myvar_dump( $wishcmpr_user_data, '$wishcmpr_user_data2' );

	if ( empty( $prod_type_local ) ) {
		$prod_type_local = false;
	};

	if ( $prod_type_local === false ) {
		?>
		<script>
			jQuery(document).ready(function ($) {
				$('#wishlist_place').after(wish_block);
				$('#wishlist_place').remove();
			});
		</script>
		<div id="wishlist_place"></div>
		<?php
		return false;
	};



	$orderby = 'title'; // or 'title' date
	$order = 'ASC'; // ASC DESC

	$query_args = array( 'posts_per_page' => - 1,
		'order' => $order,
		'orderby' => $orderby,
		'no_found_rows' => 1,
		'post_status' => 'publish',
		'post_type' => array( 'product', 'product_variation' ),
		'post__in' => $prod_type_local

	);
	$my_products = new WP_Query( $query_args );

	$howmany_products = $my_products->post_count;
//  myvar_dump($my_products,'$my_products show_my_wishitems');

	if ( $howmany_products < 1 ) {
		?>
		<script>
			jQuery(document).ready(function ($) {
				$('#wishlist_place_2').after(wish_block);
				$('#wishlist_place_2').remove();
				$('#no_wish>div').first().html('К сожалению, заданному условию не удовлетворяет ни один товар');
			});
		</script>
		<div id="wishlist_place_2"></div>
		<?php
		return false;
	};

	?>
	<div class="wishlist">
	<script id="replacing_script_3">
		jQuery(document).ready(function ($) {
			$('#wishlist_place_3').after(wish_header_block);
			$('#wishlist_place_3').remove();
			$('#no_wish').remove();
			$('#replacing_script_3').remove();
		});
	</script>
	<div id="wishlist_place_3"></div>
	<?php



	while ( $my_products->have_posts() ) : $my_products->the_post();

		$theid = get_the_ID();

//    myvar_dump($theid,'$theid');
//    $single_product = new WC_Product($theid);
//    $single_product = get_post($theid);
		$single_product = wc_get_product( $theid );
		// its a variable product
//    if( get_post_type() <> 'product_variation' ){

		if ( get_post_type() == 'product' ) {

			$product_post = $single_product->get_data();
//        myvar_dump($product_post,'$product_post');
			//        $thumb_img_url=get_the_post_thumbnail($product_post->ID);  // This will give you the  full <img> tag

			$this_post_thumbnail_id = get_post_thumbnail_id( $theid );
			$thumb_img_url = wp_get_attachment_image_src( $this_post_thumbnail_id, 'thumbnail' );
//        $thumb_img_url = wp_get_attachment_image_src($this_post_thumbnail_id, 'medium');
			$full_img_url = wp_get_attachment_image_src( $this_post_thumbnail_id, 'full' );

			$cats_dscr = '';
			if ( count( $product_post[ 'category_ids' ] ) ) {
				foreach ( $product_post[ 'category_ids' ] as $nth_cat_id ) {
					$cat_instance = get_term_by( 'id', $nth_cat_id, 'product_cat' );
//                myvar_dump($cat_instance,'$cat_instance');
					$cats_dscr .= ' <a href="/product-category/' . $cat_instance->slug . '">'
						. $cat_instance->name . '</a>' . chr( 10 );
				};
			};
			$cats_dscr = trim( $cats_dscr );
			if ( empty( $cats_dscr ) ) {
				$cats_dscr = 'Нет категорий';
			};
			//    myvar_dump($prod_img_url,'$prod_img_url');
			//    myvar_dump($thumb_img_url,'$thumb_img_url');

			$nophoto_addr = this_theme_url() . 'img/no_photo.jpg';


			if ( ! empty( $full_img_url[ 0 ] ) && ! empty( $thumb_img_url[ 0 ] ) ) {
				$img_html = '<a href="' . $full_img_url[ 0 ] . '"><img src="' . $thumb_img_url[ 0 ] . '"></a>' . chr( 10 );
			} else {
				$img_html = '<img src="' . $nophoto_addr . '">' . chr( 10 );
			};

			if ( mb_strlen( $product_post[ 'name' ], 'utf8' ) > 24 ) {
				$small_font_wedge = ' class="long_prod_name"';
			} else {
				$small_font_wedge = '';
			};


			if ( empty( $product_post[ 'name' ] ) ) {
				$post_ttl_dequoted = 'Нет названия';
			} else if ( strpos( '"', $product_post[ 'name' ] ) !== false ) {
				$post_ttl_dequoted = htmlentities( $product_post[ 'name' ], ENT_QUOTES, 'UTF-8' );
			} else {
				$post_ttl_dequoted = $product_post[ 'name' ];
			};
			$post_ttl_dequoted = strip_tags( $post_ttl_dequoted );
			if ( mb_strlen( $post_ttl_dequoted, 'utf-8' ) > 223 ) {
				$post_ttl_dequoted = mb_substr( $post_ttl_dequoted, 0, 220, 'utf-8' ) . '&hellip;';
			};


			if ( empty( $product_post[ 'description' ] ) ) {
				$post_cnt_dequoted = 'Нет описания';
			} else if ( strpos( '"', $product_post[ 'description' ] ) !== false ) {
				$post_cnt_dequoted = htmlentities( $product_post[ 'description' ], ENT_QUOTES, 'UTF-8' );
			} else {
				$post_cnt_dequoted = $product_post[ 'description' ];
			};
			$post_excerpt_dequoted = mb_substr( strip_tags( $post_cnt_dequoted ), 0, 300, 'utf-8' );

			$regular_price = $single_product->get_regular_price();
//        $sale_price=$single_product->get_sale_price();
			$usual_price = $single_product->get_price();

			$final_price = $usual_price;
			$final_price .= ( $regular_price <> $usual_price ? ' (обычная цена ' . $regular_price . ')' : '' );

			$this_in_cart = rasolo_woo_in_cart( $theid );

			?>
			<div class="wish_item" id="wish-item-<?php echo $theid; ?>">
			<div title="<?php echo 'Идентификатор товара ' . $theid; ?>"><a
					href="/wishlist/?remove_from_wishlist=<?php echo $theid;
					?>" class="remove remove_from_wishlist" title="Убрать этот продукт из списка"><i class="fa fa-times"></i></a>
			</div>
			<div>
				<?php echo $img_html; ?>
			</div>
			<div title="<?php echo $post_excerpt_dequoted; ?>"><span><a href="/product/<?php
					echo $product_post[ 'slug' ]; ?>"><?php echo $post_ttl_dequoted; ?></a></span></div>
			<div><span><?php echo $cats_dscr; ?></span></div>
			<div><span><?php echo $final_price; ?></span></div>
			<div title="<?php
			echo( $this_in_cart ?
				'Нажмите на кнопку, если хотите добавить еще один экземпляр товара в корзину' :
				'В корзине еще нет ни одного экземляра этого товара' );
			?>"><span><a href="<?php
					// echo this_request_url();
					?>/?add-to-cart=<?php echo $theid;
					?>"><?php
						echo( $this_in_cart ? 'Товар уже в корзине' : 'Добавить в корзину' );
						?></a></span>
			</div>

			</div>
		<?php

		};


	endwhile;

	?></div>
	<?php

	wp_reset_query();


}; // The end of show_my_wishitems

