package com.beloko.opengames;

import android.app.Fragment;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;

public class OnlineFragment extends Fragment{
	String LOG = "OnlineFragment";


	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);


	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		View mainView = inflater.inflate(R.layout.fragment_online, null);

		Button amazon = (Button)mainView.findViewById(R.id.amazon_button);
		amazon.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				String url = "http://www.amazon.com/gp/mas/dl/android?s=Beloko%20Games";
				Intent i = new Intent(Intent.ACTION_VIEW);
				i.setData(Uri.parse(url));
				startActivity(i);
			}
		});


		Button facebook = (Button)mainView.findViewById(R.id.facebook_button);
		facebook.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				String url = "http://www.facebook.com/BelokoGames";
				Intent i = new Intent(Intent.ACTION_VIEW);
				i.setData(Uri.parse(url));
				startActivity(i);
			}
		});
		
		Button youtube = (Button)mainView.findViewById(R.id.youtube_button);
		youtube.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				String url = "http://www.youtube.com/BelokoGames";
				Intent i = new Intent(Intent.ACTION_VIEW);
				i.setData(Uri.parse(url));
				startActivity(i);
			}
		});
		return mainView;
	}

	@Override
	public void onHiddenChanged(boolean hidden) {

		super.onHiddenChanged(hidden);
	}




}
