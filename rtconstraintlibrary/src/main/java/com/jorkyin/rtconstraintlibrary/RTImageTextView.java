package com.jorkyin.rtconstraintlibrary;

import android.content.Context;
import android.content.res.TypedArray;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class RTImageTextView extends RelativeLayout {
    public RTImageTextView(Context context, AttributeSet attrs) {
        super(context, attrs);

        LayoutInflater.from(context).inflate(R.layout.item_image_text, this, true);
        // 顶部菜单左边按钮
        ImageView mImage = findViewById(R.id.imageText_image);
        // 顶部菜单文字
        TextView mText = findViewById(R.id.imageText_text);

        TypedArray attributes = context.obtainStyledAttributes(attrs,R.styleable.RT_ImageText_View);
        if (attributes != null){
            int imageDrawable = attributes.getResourceId(R.styleable.RT_ImageText_View_image_src,R.mipmap.icon_iamge);
            if (imageDrawable != -1){
                mImage.setImageResource(imageDrawable);
            }
            String titleText=attributes.getString(R.styleable.RT_ImageText_View_image_text);
            if (!TextUtils.isEmpty(titleText)){
                mText.setText(titleText);
            }
            attributes.recycle();
        }
    }
}
