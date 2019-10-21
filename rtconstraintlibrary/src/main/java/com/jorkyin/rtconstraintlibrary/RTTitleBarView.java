package com.jorkyin.rtconstraintlibrary;

import android.content.Context;
import android.content.res.TypedArray;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;


public class RTTitleBarView extends RelativeLayout implements View.OnClickListener {

    private ImageView mLeftImage;// 设置菜单左边图片
    private TextView    mLeftText;// 设置菜单左边按钮文字
    private TextView    mTitleText;// 设置菜单右边按钮文字
    private TextView    mRightText;// 设置菜单右边按钮文字
    private ImageView   mRightImage;// 设置菜单右边图片


    //https://www.cnblogs.com/whoislcj/p/5714760.html
    public RTTitleBarView(Context context, AttributeSet attrs) {
        super(context, attrs);
        LayoutInflater.from(context).inflate(R.layout.item_title_menu, this, true);
        mLeftImage = findViewById(R.id.left_image);// 菜单左边图片
        mLeftText = findViewById(R.id.left_text);// 菜单左边按钮文字
        mTitleText = findViewById(R.id.title_text);// 菜单中间
        mRightText = findViewById(R.id.right_text); // 菜单右边按钮文字
        mRightImage = findViewById(R.id.right_image);// 菜单右边图片

        TypedArray attributes = context.obtainStyledAttributes(attrs, R.styleable.RT_TitleMenu_View);
        if (attributes != null) {
            int  leftImageDrawable = attributes.getResourceId(R.styleable.RT_TitleMenu_View_left_image_src,-1);
            if (leftImageDrawable != -1) {
                mLeftImage.setVisibility(View.VISIBLE);
                mLeftImage.setImageResource(leftImageDrawable);
                mLeftImage.setOnClickListener(this);
            }

            String leftText = attributes.getString(R.styleable.RT_TitleMenu_View_left_text);
            if (!TextUtils.isEmpty(leftText)) {
                this.mLeftText.setText(leftText);
            }

            String titleText = attributes.getString(R.styleable.RT_TitleMenu_View_title_text);
            if (!TextUtils.isEmpty(titleText)) {
                this.mTitleText.setText(titleText);
            }

            String rightText = attributes.getString(R.styleable.RT_TitleMenu_View_right_text);
            if (!TextUtils.isEmpty(rightText)) {
                this.mRightText.setText(rightText);
            }

            int rightImageDrawable = attributes.getResourceId(R.styleable.RT_TitleMenu_View_right_image_src, -1);
            if (rightImageDrawable != -1) {
                mRightImage.setVisibility(View.VISIBLE);
                mRightImage.setImageResource(rightImageDrawable);
                mLeftImage.setOnClickListener(this);
            }

            attributes.recycle();
        }
    }

   OnItemClickListener mOnClickListener;
    public void setTitleClickListener(OnItemClickListener onClickListener) {
        mOnClickListener=onClickListener;
    }

    public interface OnItemClickListener {
        void onItemClick(View v);
    }

    @Override
    public void onClick(View v) {
        if (mOnClickListener!=null){
            mOnClickListener.onItemClick(v);
        }
    }
}
