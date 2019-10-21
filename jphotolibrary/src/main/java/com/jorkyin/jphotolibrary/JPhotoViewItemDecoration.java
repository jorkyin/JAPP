package com.jorkyin.jphotolibrary;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.text.TextPaint;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.jorkyin.jphotolibrary.util.JDensityUtil;

public class JPhotoViewItemDecoration extends RecyclerView.ItemDecoration {
    private static final String TAG = "RTPhotoItemDecoration";
    private DecorationCallback callback;
    private TextPaint mTextPaint;
    private Paint mGroutPaint;
    private int topGap;
    private int alignBottom;
    private Context mContext;
    private int mSpanCount = -1;//多少列

    public JPhotoViewItemDecoration(Context context, int spanCount, DecorationCallback decorationCallback) {
        this.mContext = context;
        this.mSpanCount = spanCount;
        Resources res = context.getResources();
        this.callback = decorationCallback;
        //设置悬浮栏的画笔---paint
        mGroutPaint = new Paint();
        mGroutPaint.setColor(res.getColor(R.color.colorGray));

        //设置悬浮栏中文本的画笔
        mTextPaint = new TextPaint();
        mTextPaint.setAntiAlias(true);
        mTextPaint.setTextSize(JDensityUtil.dip2px(context, 14));
        mTextPaint.setColor(Color.DKGRAY);
        mTextPaint.setTextAlign(Paint.Align.LEFT);
        //决定悬浮栏的高度等
        topGap = res.getDimensionPixelSize(R.dimen.sectioned_top);
        //决定文本的显示位置等
        alignBottom = res.getDimensionPixelSize(R.dimen.sectioned_alignBottom);
    }

    /**
     * 判断是不是组中的第一个排
     *
     * @param pos
     * @return
     */
    private boolean isFirstInGroup(int pos, int spanCount) {
        return callback.getGroupId(pos) <= spanCount;
    }

    //最后一列
    private boolean isLastColumnInGroup(int pos, int spanCount) {
        return callback.getGroupId(pos) % spanCount == 0;
    }
    // 作用：设置ItemView的内嵌偏移长度（inset）
    @Override
    public void getItemOffsets(@NonNull Rect outRect, @NonNull View view, @NonNull RecyclerView parent, @NonNull RecyclerView.State state) {
        super.getItemOffsets(outRect, view, parent, state);
        // 参数说明：
        // 1. outRect：全为 0 的 Rect（包括着Item）
        // 2. view：RecyclerView 中的 视图Item
        // 3. parent：RecyclerView 本身
        // 4. state：状态
        //如果不是第一个，则设置top的值。

        int pos = parent.getChildAdapterPosition(view);//当前Item
        //  Log.i(TAG, "onStart: pos=" + pos);
        if (isFirstInGroup(pos, mSpanCount)) {//只有是同一组的第一排才显示悬浮栏
            outRect.top = topGap;
        } else {
            outRect.top = JDensityUtil.dip2px(mContext, 1);
        }

        outRect.left = JDensityUtil.dip2px(mContext, 1);
        if (isLastColumnInGroup(pos, mSpanCount)) {//如果是最后一列
            outRect.right = JDensityUtil.dip2px(mContext, 1);
        }
    }

    // 作用：在子视图上设置绘制范围，并绘制内容
    // 类似平时自定义View时写onDraw()一样
    // 绘制图层在ItemView以下，所以如果绘制区域与ItemView区域相重叠，会被遮挡
    @Override
    public void onDraw(@NonNull Canvas c, @NonNull RecyclerView parent, @NonNull RecyclerView.State state) {
        super.onDraw(c, parent, state);
    }

    // 作用：同样是绘制内容，但与onDraw（）的区别是：绘制在图层的最上层
    @Override
    public void onDrawOver(@NonNull Canvas c, @NonNull RecyclerView parent, @NonNull RecyclerView.State state) {
        super.onDrawOver(c, parent, state);
        int childCount = parent.getChildCount();//每个组中的Item个数.
        final int itemCount = state.getItemCount();
        final int left = parent.getLeft() + parent.getPaddingLeft();
        final int right = parent.getRight() - parent.getPaddingRight();
        //标记上一个item对应的Group
        String preGroupName;
        //当前item对应的Group
        String currentGroupName = " ";
        //遍历所有的子View（所有的Item）
        for (int i = 0; i < childCount; i++) {
            View view = parent.getChildAt(i);
            int position = parent.getChildAdapterPosition(view);
            preGroupName = currentGroupName;
            currentGroupName = callback.getGroupString(position);//获取当前的item组名字
            if (currentGroupName == null || currentGroupName.equals(preGroupName)) {
                continue;
            }

            int viewBottomHeigt = view.getBottom();//获取需要操作的ItemDecoration的底部距离屏幕顶部的高度

            float bottom = Math.max(topGap, view.getTop());//决定当前顶部第一个悬浮ItemDecoration的bottom
            //下一XX省的ItemDecoration一步步逼近我们的首部的悬浮ItemDecoration
            if (position + mSpanCount < itemCount) {
                //获取下个GroupName
                String nextGroupName = callback.getGroupString(position + mSpanCount);
                //下一组的第一个View（ItemDecoration）接近头部
                if (!currentGroupName.equals(nextGroupName) && viewBottomHeigt < bottom) {
                    //bottom最小等于mGroupHeight，而当viewBottom（view.getBottom()）小于bottom时，说明当前首都悬浮的ItemDecoration已经被挤压。不断更新bottom值。（因为我们绘制它时需要坐标信息，也就是这个bottom）
                    bottom = viewBottomHeigt;
                }
            }
            //根据bottom绘制ItemDecoration
            c.drawRect(left, bottom - topGap, right, bottom, mGroutPaint);
            Paint.FontMetrics fm = mTextPaint.getFontMetrics();
            //文字竖直居中显示
            float baseLine = bottom - (topGap - (fm.bottom - fm.top)) / 2 - fm.bottom;
            c.drawText(currentGroupName, left, baseLine, mTextPaint);
        }
    }


    //定义一个借口方便外界的调用
    public interface DecorationCallback {
        int getGroupId(int position);//当前组的第几个
        String getGroupString(int position);
    }
}
