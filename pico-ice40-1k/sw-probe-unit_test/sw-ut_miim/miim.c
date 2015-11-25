#include "miim.h"
#include "hardware.h"

extern void delay(__IO u32 nCount);

u32 detect(int n)
{
    return ( mdio_read(PHYREG_ID1, n) ); 
}

u32 link(void)
{
    return ((mdio_read(PHYREG_STATUS, PHY_ADDR)>>SVAL)&0x01); 
}

void set_link(SetLink_Type mode)
{
   u32 val=0;
   
   if( mode == CNTL_AUTONEGO)
   {    
      val = CNTL_AUTONEGO; 
   }
   else
   {
        val = (mode & (CNTL_SPEED|CNTL_DUPLEX)); 
   }

    mdio_write(PHYREG_CONTROL, val);

}

static void ll_clk(int v)
{
  if (v)
    //reg_wr((MM_GPIOB + 0x800 + 0x200) , 0);
    MM_WR( (MM_GPIOB + 0x800 + 0x200) , 0 );
  else
    //reg_wr((MM_GPIOB + 0x800 + 0x200) , (1 << 15));
    MM_WR( (MM_GPIOB + 0x800 + 0x200) , (1 << 15) );
}

static void ll_dat(int v)
{
    if (v)
        MM_WR( (MM_GPIOB + 0x800 + 0x100) , (1 << 14) );
    else
        MM_WR( (MM_GPIOB + 0x800 + 0x100) , 0 );
}

void output_MDIO(u32 val, u32 n)
{
    for(val <<= (32-n); n; val<<=1, n--)
    {
        if(val & 0x80000000)
            ll_dat(1);
        else
            ll_dat(0);

        delay(1);
        ll_clk(1);
        delay(1);
        ll_clk(0);
    }
}

u32 input_MDIO()
{
    u32 i, val=0; 
    for(i=0; i<16; i++)
    {
        val <<=1;
        /* Clock pulse */
        ll_clk(1);
        delay(1);
        ll_clk(0);
        delay(1);
        /* Read the next bit */
        if ( *(volatile u32 *)MM_GPIOB & (1 << 14) )
            val |= 1;
    }
    return (val);
}

void turnaround_MDIO(void)
{
    /* Clear the OUTEN bit (set gpio as input) */
    MM_WR(GPIOB_OUTENCLR, (1 << 14));

    delay(1);
    ll_clk(1);
    delay(1);
    ll_clk(0);
    delay(1);
}

void idle_MDIO(void)
{
    /* Set the OUTEN bit (set gpio as output) */
    MM_WR(GPIOB_OUTENSET, (1 << 14));

    ll_clk(1);
    delay(1);
    ll_clk(0);
    delay(1);
}
u32 mdio_read(u32 PhyRegAddr, u32 PhyAddr)
{
    u32 val =0;

    /* 32 Consecutive ones on MDO to establish sync */
    output_MDIO(0xFFFFFFFF, 32);

    /* start code 01, read command (10) */
    output_MDIO(0x06, 4);

    /* write PHY address */
    output_MDIO(PhyAddr, 5);

    output_MDIO(PhyRegAddr, 5);

    /* turnaround MDO is tristated */
    turnaround_MDIO();

    /* Read the data value */
    val = input_MDIO();

    /* turnaround MDO is tristated */
    idle_MDIO();

    return val;
}

void mdio_write(u32 PhyRegAddr, u32 val)
{

    /* 32 Consecutive ones on MDO to establish sync */
    output_MDIO(0xFFFFFFFF, 32);

    /* start code 01, write command (01) */
    output_MDIO(0x05, 4);

    /* write PHY address */
    output_MDIO(PHY_ADDR, 5);

    output_MDIO(PhyRegAddr, 5);

    /* turnaround MDO */
    output_MDIO(0x02, 2);

    /* Write the data value */
    output_MDIO(val, 16);

    /* turnaround MDO is tristated */
    idle_MDIO();
}

