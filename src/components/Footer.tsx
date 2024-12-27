import { motion } from "framer-motion"

interface FooterLegendProps {
    children: React.ReactNode;
}

const FooterLegend: React.FC<FooterLegendProps> = ({ children }) => {
    return (
        <motion.div initial={{ y: "100%" }} animate={{ y: 0 }}  style={{ position: "absolute", bottom: 0, left: 0, right: 0 }} transition={{ delay: 0.2, }} className="footer-content">      
            <p className="tooltip">
                <img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAADAAAAAwCAYAAABXAvmHAAAACXBIWXMAAAsTAAALEwEAmpwYAAACjklEQVR4nO1ZyW4TQRAdARduLCe2E/AHbDd/AFI89SpdZZL/YD04h+BciQDxD2xCJArfAcoNLiwHUGKuOFyCHrFZZEUznmnPDMJPKsmS5Zn3uqq7qp+TZIYZZiiNbrd7QMSuiPptwJ4JbBOwrwL7zhh+3hT1p4DdUu1c5m+SuiGyeFrUVgT+Ceq7k4V9FPVeCOFU5cTTND0O+COB70xOfCwGgD8MIRyrhLzMWwfq2xGI/x2wLdVgUyPearUOcdWjE9cxIQ/4rqjkQwiHAVubOnndC1F/yXfGIn8Qas+rIo/fmViLkolKykb3y4Tdj7FhayGPkQjpzBciz2ONJ0PdAqC+zWO7htKxdTYpNjvANiotJWGHLdmk+IzR89ptP1MyC4O5uc7J/ALUVsrXblQBu4DdzX1sFpttxl64QRF75O1V6QWBv881AHKqLE1+SpGGcClTAOB36iaK/bKgfiNbQB1dV/OGPckuIV5GYqwW/G2a+lkGP0cRAH+TQ4D3I6W79+uZ6r04Amwrj4CdSAKW/hCwFKmMBv+FgH5jBSBXCUXaxFMR4NmbmLZIgwU8zs4AvZ3GCrDrmQJoOjVVgGrnQqYADkxQ+9A0AZJ3mIvVeKILUFvORf7nC3mLomPWHAGDiS40BO2+xowSsNWkmP9Z/FLPAS6EcA64dh5q74oLsC9XFxaOJkVAr7Js7ZaNVB1JGdCrrIu8qN1LoliLkbpzLdYiQaOVhmuF5F9EM3dH4GrQYKqA/CqznkwL9Cp5MsQnb59Lb9i84LE2zMag9EaFfeOqt9vtI0nVYHekY1ZkduJsI2rLIosnkrrBIYumE+A3aX3w4sGbHa+nw+gL7DW/o7cjYhcb8TfrDDMk/z5+AHLrqx1z8KkFAAAAAElFTkSuQmCC"/>
                Millennium is not affiliated with Steam®, Valve, or any of their partners.
            </p>       
            {children}
        </motion.div>
    )
}

export { FooterLegend }