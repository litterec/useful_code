<?php

/*
  Software Name: Rasolo Wish Compare
  Software URI: http://design.ra-solo.ru
  Description: The Wordpress addition. It allows to maintain both the compare and wish lists
  Version: 1.0
  Author: Andrew Galagan
  Author URI: http://galagan.ra-solo.ru/?showitem=6&lang=eng
 */

/*  Copyright 2018  Andrew Galagan  (email: andrew.galagan@gmail.com)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

class RasoloWishCompare {

    static public $WISH_MAX_SIZE = 4;
    static public $CMPR_MAX_SIZE = 4;
    static public $COOKIE_KEY = '_wishcmpr_cookie';
    static public $META_KEY = '_rasolo_wishcompare_meta';

    private $wish_ids;
    private $cmpr_ids;
    private $error;
    private $already_read_meta;
    private $already_read_cookies;
    private $last_added_wish_key;
    private $last_added_cmpr_key;
    private $last_rmvd_wish_key;
    private $last_rmvd_cmpr_key;

    function __construct(){

        $this->wish_ids= array();
        $this->cmpr_ids= array();
        $this->error= array();
        $this->already_read_meta=false;
        $this->already_read_cookies=false;
        $this->last_added_wish_key=false;
        $this->last_added_cmpr_key=false;
        $this->last_rmvd_wish_key=false;
        $this->last_rmvd_cmpr_key=false;

        if($this_user=get_current_user_id()){
            $this->read_user_meta($this_user);
        } else {
            $this->read_user_cookie();
        };

    }

    private function write_user_cookie($array_for_cookie){
        if(!is_array($array_for_cookie)){
            $this->error[264]='The input data for cookies is not an array';
            return;
        };

        $string_for_cookie=urlencode(json_encode($array_for_cookie));
        $exp_time=intval(time())+3600*24*7;

//        if(!empty($array_for_cookie['cmpr_ids'])){
//        myvar_dump($string_for_cookie,'$list_of_cmpr_id 5347327');
//        die('let us write cookie');
//            }

        setcookie(self::$COOKIE_KEY,$string_for_cookie,$exp_time,'/');
        return;
    }

    private function write_user_meta($usr,$array_for_meta){
        $usr_meta_data_ser=serialize($array_for_meta);
        update_user_meta( $usr, self::$META_KEY, $usr_meta_data_ser );
        return;
    }

    private function read_user_meta($some_user){

        if($this->already_read_meta){
            return;
        };
        $this->already_read_meta=true;
        $usr_meta_ser=get_user_meta( $some_user, self::$META_KEY, true );

        $usr_wishcompare_meta=stripslashes($usr_meta_ser);

        $this_user_meta=@unserialize($usr_wishcompare_meta);

        if(isset($this_user_meta['wish_ids'])){
            if(is_array($this_user_meta['wish_ids'])){
                $this->wish_ids=array_filter($this_user_meta['wish_ids'],'is_numeric');
            } else {
                $this->wish_ids=array();
                $this->error[401]='The source meta wish data is corrupt!';
            };

        } else {
            $this->clear_user_meta();
            $this->wish_ids=array();
            $this->error[242]='Wish ids are absent in the user meta:'.$usr_meta_ser.'{==';
        };
        if(isset($this_user_meta['cmpr_ids'])){
            if(is_array($this_user_meta['wish_ids'])){
                $this->cmpr_ids=array_filter($this_user_meta['cmpr_ids'],'is_numeric');
            } else {
                $this->clear_user_meta();
                $this->wish_ids=array();
                $this->error[401]='The source meta cmpr data is corrupt!';
            };
        } else {
            $this->clear_user_meta();
            $this->cmpr_ids=array();
            $this->error[243]='Compare ids are absent in the user meta:'.$usr_meta_ser.'{===';
        };

    }

    private function read_user_cookie(){

        if($this->already_read_cookies){
            return;
        };
        $this->already_read_cookies=true;

        if(isset($_COOKIE[self::$COOKIE_KEY])){

//            echo '$_COOKIE[WHOLESALE_COOKIE_KEY]='.$_COOKIE[self::$COOKIE_KEY].'{==';

            $cookie_wo_slashes=stripslashes($_COOKIE[self::$COOKIE_KEY]);
            $cookie_url_decoded=urldecode($cookie_wo_slashes);
            $wishlist_local_obj=@json_decode($cookie_url_decoded);

            $wishlist_local=(array)$wishlist_local_obj;

            if(isset($wishlist_local['wish_ids'])){
                if(is_array($wishlist_local['wish_ids'])){

//                    myvar_dump($wishlist_local,'$wishlist_local inside 001');

                    $this->wish_ids=array_map('intval',$wishlist_local['wish_ids']);

                } else {
                    $this->wish_ids=array();
                    $this->error[299]='The source cookie wish data is corrupt!';
                };
            } else {
                $this->wish_ids=array();
                $this->error[244]='The wish ids data are absent in the user cookies';
            };
            if(isset($wishlist_local['cmpr_ids'])){
                if(is_array($wishlist_local['cmpr_ids'])){
                    $this->cmpr_ids=array_map('intval',$wishlist_local['cmpr_ids']);
                } else {
                    $this->cmpr_ids=array();
                    $this->error[299]='The source cookie cmpr data is corrupt!';

                };
            } else {
                $this->cmpr_ids=array();
                $this->error[247]='The compare ids data is absent in the user cookies';
            };

        } else {
            $this->error[245]='Ids data is absent in the user cookies';
        };

    }

    private function clear_user_meta(){
        if($this_doomed_user=get_current_user_id()){
            $this->wish_ids= array();
            $this->cmpr_ids= array();
            $another_empty_array=array('wish_ids'=>array(),'cmpr_ids'=>array());
            $this->write_user_meta($this_doomed_user,$another_empty_array);

//            delete_user_meta( $this_doomed_user, self::$META_KEY );
        } else {
            $this->error[40092]='There was an attempt to clear meta while user is logged out';
        };


    }

    public function add_wish($new_wish_id){
        if(!is_numeric($new_wish_id)){
            $this->error[403]='Incorrect new wish item type';
            return;
        };
        if(in_array($new_wish_id,$this->wish_ids)){
            $this->error[404]='The item is already in the wish list';
            return;
        };
        if(count($this->wish_ids)+1>self::$WISH_MAX_SIZE){
            $this->error[405]='The wish ids memory area capacity is exceeded';
            return;

        };
        $this->wish_ids[]=intval($new_wish_id);
        end($this->wish_ids);
        $this->last_added_wish_key=key($this->wish_ids);
    }

    public function add_cmpr($new_cmpr_id){
        if(!is_numeric($new_cmpr_id)){
            $this->error[407]='Incorrect new cmpr item type';
            return;
        };
        if(in_array($new_cmpr_id,$this->cmpr_ids)){
            $this->error[408]='The item is already in the cmpr list';
            return;
        };
        if(count($this->cmpr_ids)+1>self::$CMPR_MAX_SIZE){
            $this->error[409]='The cmpr ids memory area capacity is exceeded';
            return;

        };
        $this->cmpr_ids[]=intval($new_cmpr_id);
        end($this->cmpr_ids);
        $this->last_added_cmpr_key = key($this->cmpr_ids);
    }

    public function write_wish(){  // this shit was in parameters $list_of_wish_id=array()
//        if(empty($list_of_wish_id)){
        if($this_user=get_current_user_id()){
            $this->write_user_meta($this_user,array('wish_ids'=>$this->wish_ids,'cmpr_ids'=>$this->cmpr_ids));
        } else {
            $this->write_user_cookie(array('wish_ids'=>$this->wish_ids,'cmpr_ids'=>$this->cmpr_ids));
        };

//        };
/*  This is shit!s

        if(!is_array($list_of_wish_id)){
            $this->error[255]='Icorrect output wish data';
            return;
        };
        $local_pure_numeric=array_filter($list_of_wish_id,'is_numeric');

        $this->wish_ids=$local_pure_numeric;
        if($this_user=get_current_user_id()){
            $this->write_user_meta($this_user,array('wish_ids'=>$local_pure_numeric,'cmpr_ids'=>$this->cmpr_ids));
        } else {
            $this->write_user_cookie(array('wish_ids'=>$local_pure_numeric,'cmpr_ids'=>$this->cmpr_ids));
        };

*/
    }

    public function write_cmpr(){  // This is the shit $list_of_cmpr_id=array()

//        if(empty($list_of_cmpr_id)){
        if($this_user=get_current_user_id()){
            $this->write_user_meta($this_user,array('wish_ids'=>$this->wish_ids,'cmpr_ids'=>$this->cmpr_ids));
        } else {
            $this->write_user_cookie(array('wish_ids'=>$this->wish_ids,'cmpr_ids'=>$this->cmpr_ids));
        };
//        };

/*  This is the shit too
        if(!is_array($list_of_cmpr_id)){
            $this->error[256]='Icorrect output cmpr data';
            return;
        };
        $local_pure_numeric=array_filter($list_of_cmpr_id,'is_numeric');
        $this->cmpr_ids=$local_pure_numeric;
        if($this_user=get_current_user_id()){
            $this->write_user_meta($this_user,array('wish_ids'=>$this->wish_ids,'cmpr_ids'=>$list_of_cmpr_id));
        } else {
            $this->write_user_cookie(array('wish_ids'=>$this->wish_ids,'cmpr_ids'=>$list_of_cmpr_id));
        };
*/

    }

    public function remove_wish($some_wish_id){
        if(!is_numeric($some_wish_id)){
            $this->error[273]='The input wish item is not numeric';
            return;
        };
        if(!in_array($some_wish_id,$this->wish_ids)){
            $this->error[274]='The input wish item is not in array so far';
            return;
        };
        $this->last_rmvd_wish_key=array_search($some_wish_id,$this->wish_ids);
        unset($this->wish_ids[intval($this->last_rmvd_wish_key)]);
    }

    public function remove_cmpr($some_cmpr_id){
        if(!is_numeric($some_cmpr_id)){
            $this->error[271]='The input compare item is not numeric';
            return;
        };
        if(!in_array($some_cmpr_id,$this->cmpr_ids)){
            $this->error[272]='The input compare item is not in array so far';
            return;
        };
        $this->last_rmvd_cmpr_key=array_search($some_cmpr_id,$this->cmpr_ids);
        unset($this->cmpr_ids[intval($this->last_rmvd_cmpr_key)]);
    }
    public function in_wish_list($some_wish_id){
        return in_array($some_wish_id,$this->wish_ids);
    }
    public function in_cmpr_list($some_cmpr_id){
        return in_array($some_cmpr_id,$this->cmpr_ids);
    }
    public function get_wish(){
        return $this->wish_ids;
    }
    public function get_cmpr(){
        return $this->cmpr_ids;
    }
    public function how_many_errors(){
        return count($this->error);
    }
    public function get_errors(){
        return $this->error;
    }

}

if ( ! defined( 'ABSPATH' ) ) {
	exit;
};